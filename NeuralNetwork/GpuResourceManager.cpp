#include <atomic>
#include <combaseapi.h>
#include <cstdint>
#include <cstring>
#include <d3d12.h>
#include <d3dx12_core.h>
#include <memory>
#include <rpcndr.h>
#include <string>
#include <utility>
#include <vector>
#include <Windows.h>
#include <wrl\client.h>
#include "ConstantBuffer.h"
#include "DataStructures.h"
#include "GpuResource.h"
#include "GpuResourceManager.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "VertexBuffer.h"

namespace {
	std::atomic<uint64_t> g_resourceIdCounter{ 0 };
	std::string GenerateUniqueResourceId( const std::string& prefix = "Resource" )
	{
		return prefix + "_" + std::to_string( g_resourceIdCounter.fetch_add( 1 ) );
	}
}

GpuResourceManager::GpuResourceManager( Renderer& renderer )
{
	this->renderer = &renderer;
}

GpuResourceManager::~GpuResourceManager()
{
	resourceHeap.clear();
	perFrameResourceHeap.clear();
}

bool GpuResourceManager::RegisterResource( std::unique_ptr<GpuResource> resource )
{
	std::string id = GenerateUniqueResourceId();
	resource->SetResourceId( id );
	return resourceHeap.emplace( id, std::move( resource ) ).second;
}

bool GpuResourceManager::RegisterPerFrameResource( const GpuResource& resource )
{
	std::string id = GenerateUniqueResourceId();
	std::vector<std::unique_ptr<GpuResource>> resources;
	auto framesInFlight = Renderer::BackBufferCount;
	resources.reserve( framesInFlight );
	for( unsigned int i = 0; i < framesInFlight; ++i ) {
		auto clone = resource.Clone( *renderer );
		if( !clone ) return false;
		clone->SetResourceId( id + "_frame" + std::to_string( i ) );
		resources.push_back( std::move( clone ) );
	}
	return perFrameResourceHeap.emplace( id, std::move( resources ) ).second;
}

GpuResource* GpuResourceManager::GetResource( const ResourceID& id )
{
	auto perFrameIt = perFrameResourceHeap.find( id );
	if( perFrameIt != perFrameResourceHeap.end() ) {
		unsigned int frameIndex = renderer->GetCurrentFrameIndex();
		if( frameIndex < perFrameIt->second.size() )
			return perFrameIt->second[frameIndex].get();
	}

	auto singleIt = resourceHeap.find( id );
	return (singleIt != resourceHeap.end()) ? singleIt->second.get() : nullptr;
}

void GpuResourceManager::RemoveResource( const ResourceID& id )
{
	resourceHeap.erase( id );
	perFrameResourceHeap.erase( id );
}

void GpuResourceManager::CreateVertexBuffer( const std::vector<Vertex>& vertices, const std::string& debugName )
{
	auto device = renderer->GetDevice();
	uint bufferSize = static_cast<uint>(vertices.size() * sizeof( Vertex ));

	// Create the default heap resource (GPU local)
	CD3DX12_HEAP_PROPERTIES heapProps( D3D12_HEAP_TYPE_DEFAULT );
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );

	Microsoft::WRL::ComPtr<ID3D12Resource> vbResource;
	HRESULT hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS( &vbResource )
	);
	if( FAILED( hr ) ) return;

	// Create the upload heap
	CD3DX12_HEAP_PROPERTIES uploadHeapProps( D3D12_HEAP_TYPE_UPLOAD );
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;
	hr = device->CreateCommittedResource(
		&uploadHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS( &uploadResource )
	);
	if( FAILED( hr ) ) return;

	// Copy data to upload heap
	void* mapped = nullptr;
	uploadResource->Map( 0, nullptr, &mapped );
	memcpy( mapped, vertices.data(), bufferSize );
	uploadResource->Unmap( 0, nullptr );

	// Create and register the GpuResource
	auto vb = std::make_unique<VertexBuffer>( vertices, debugName );
	vb->SetResource( std::move( vbResource ) );
	vb->SetUploadResource( std::move( uploadResource ) );
	vb->SetResourceSize( bufferSize );
	RegisterResource( std::move( vb ) );
}

void GpuResourceManager::CreateIndexBuffer( const std::vector<uint>& indices, const std::string& debugName )
{
	auto device = renderer->GetDevice();
	uint bufferSize = static_cast<uint>(indices.size() * sizeof( uint ));

	CD3DX12_HEAP_PROPERTIES heapProps( D3D12_HEAP_TYPE_DEFAULT );
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );

	Microsoft::WRL::ComPtr<ID3D12Resource> ibResource;
	HRESULT hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS( &ibResource )
	);
	if( FAILED( hr ) ) return;

	CD3DX12_HEAP_PROPERTIES uploadHeapProps( D3D12_HEAP_TYPE_UPLOAD );
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;
	hr = device->CreateCommittedResource(
		&uploadHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS( &uploadResource )
	);
	if( FAILED( hr ) ) return;

	void* mapped = nullptr;
	uploadResource->Map( 0, nullptr, &mapped );
	memcpy( mapped, indices.data(), bufferSize );
	uploadResource->Unmap( 0, nullptr );

	auto ib = std::make_unique<IndexBuffer>( indices, debugName );
	ib->SetResource( std::move( ibResource ) );
	ib->SetUploadResource( std::move( uploadResource ) );
	ib->SetResourceSize( bufferSize );
	RegisterResource( std::move( ib ) );
}

void GpuResourceManager::CreateConstantBuffer( const std::vector<byte>& data, const std::string& debugName )
{
	auto device = renderer->GetDevice();
	uint bufferSize = static_cast<uint>(data.size());

	CD3DX12_HEAP_PROPERTIES heapProps( D3D12_HEAP_TYPE_UPLOAD );
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );

	Microsoft::WRL::ComPtr<ID3D12Resource> cbResource;
	HRESULT hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS( &cbResource )
	);
	if( FAILED( hr ) ) return;

	void* mapped = nullptr;
	cbResource->Map( 0, nullptr, &mapped );
	memcpy( mapped, data.data(), bufferSize );
	cbResource->Unmap( 0, nullptr );

	auto cb = std::make_unique<ConstantBuffer>( data, debugName );
	cb->SetResource( std::move( cbResource ) );
	cb->SetResourceSize( bufferSize );
	RegisterResource( std::move( cb ) );
}