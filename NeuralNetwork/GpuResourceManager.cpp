#include <atomic>
#include <combaseapi.h>
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
#include "Texture.h"
#include "Types.h"
#include "UploadManager.h"
#include "VertexBuffer.h"

using ResourceID = std::string;

GpuResourceManager::GpuResourceManager( Renderer& renderer )
{
	this->renderer = &renderer;
}

GpuResourceManager::~GpuResourceManager()
{
	resourceHeap.clear();
	perFrameResourceHeap.clear();
}

bool GpuResourceManager::RegisterResource( ResourceID id, std::unique_ptr<GpuResource> resource )
{
	resource->SetResourceId( id );
	return resourceHeap.emplace( id, std::move( resource ) ).second;
}

ResourceID GpuResourceManager::GenerateUniqueResourceId( const std::string& prefix )
{
	return prefix + "_" + std::to_string( g_resourceIdCounter.fetch_add( 1 ) );
}

bool GpuResourceManager::RegisterPerFrameResource( ResourceID id, std::unique_ptr<GpuResource> resource )
{
	std::vector<std::unique_ptr<GpuResource>> resources;
	auto framesInFlight = Renderer::BackBufferCount;
	resources.reserve( framesInFlight );
	for( unsigned int i = 0; i < framesInFlight; ++i ) {
		auto clone = resource->Clone( *renderer );
		if( !clone ) return false;
		clone->SetResourceId( id + "_frame" + std::to_string( i ) );
		resources.push_back( std::move( clone ) );
	}
	return perFrameResourceHeap.emplace( id, std::move( resources ) ).second;
}

GpuResource* GpuResourceManager::GetResource( const ResourceID& id ) const
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

std::vector<GpuResource*> GpuResourceManager::GetAllResources() const
{
	std::vector<GpuResource*> allResources;
	for( auto& pair : resourceHeap ) {
		allResources.push_back( pair.second.get() );
	}
	for( auto& pair : perFrameResourceHeap ) {
		for( auto& resource : pair.second ) {
			allResources.push_back( resource.get() );
		}
	}
	return allResources;
}

void GpuResourceManager::RemoveResource( const ResourceID& id )
{
	resourceHeap.erase( id );
	perFrameResourceHeap.erase( id );
}

void GpuResourceManager::UploadResource( const ResourceID& id )
{
	auto* resource = GetResource( id );
	if( !resource ) return;

	auto* uploadManager = renderer->GetUploadManager();
	if( !uploadManager ) return;

	// Enqueue the upload request
	uploadManager->Enqueue( {
		// recordFunc: called on the upload thread, records the copy/upload
		[resource]( ID3D12GraphicsCommandList* cmdList ) {
			resource->Upload( cmdList );
			},
							// onComplete: set resource as ready
							[resource]() {
								resource->SetIsReady( true );
							}
							} );
}

ResourceID GpuResourceManager::CreateVertexBuffer( const std::vector<Vertex>& vertices, const std::string& name )
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
	if( FAILED( hr ) ) return "resource not found";

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
	if( FAILED( hr ) ) return "resource not found";

	// Create and register the GpuResource
	auto vb = std::make_unique<VertexBuffer>( vertices, name );
	vb->SetResource( std::move( vbResource ) );
	vb->SetCurrentState( D3D12_RESOURCE_STATE_COPY_DEST );
	vb->SetUploadResource( std::move( uploadResource ) );
	vb->SetResourceSize( bufferSize );

	ResourceID id = GenerateUniqueResourceId();
	RegisterResource( id, std::move( vb ) );

	// Set vertex buffer view for the resource
	auto* vbPtr = static_cast<VertexBuffer*>(GetResource( id ));
	if( vbPtr ) {
		D3D12_VERTEX_BUFFER_VIEW view = {};
		view.BufferLocation = vbPtr->GetResource()->GetGPUVirtualAddress();
		view.SizeInBytes = bufferSize;
		view.StrideInBytes = sizeof( Vertex );
		vbPtr->SetVertexBufferView( view );
	}

	return id;
}

ResourceID GpuResourceManager::CreateIndexBuffer( const std::vector<uint>& indices, const std::string& name )
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
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS( &ibResource )
	);
	if( FAILED( hr ) ) return "resource not found";

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
	if( FAILED( hr ) ) return "resource not found";

	auto ib = std::make_unique<IndexBuffer>( indices, name );
	ib->SetResource( std::move( ibResource ) );
	ib->SetCurrentState( D3D12_RESOURCE_STATE_COPY_DEST );
	ib->SetUploadResource( std::move( uploadResource ) );
	ib->SetResourceSize( bufferSize );

	ResourceID id = GenerateUniqueResourceId();
	RegisterResource( id, std::move( ib ) );

	// Set index buffer view for the resource
	auto* ibPtr = static_cast<IndexBuffer*>(GetResource( id ));
	if( ibPtr ) {
		D3D12_INDEX_BUFFER_VIEW view = {};
		view.BufferLocation = ibPtr->GetResource()->GetGPUVirtualAddress();
		view.SizeInBytes = bufferSize;
		view.Format = ibPtr->GetFormat();
		ibPtr->SetIndexBufferView( view );
	}

	return id;
}

ResourceID GpuResourceManager::CreateConstantBuffer( const std::vector<byte>& data, const std::string& name )
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
	if( FAILED( hr ) ) return "resource not found";

	auto cb = std::make_unique<ConstantBuffer>( data, name );
	cb->SetResource( std::move( cbResource ) );
	cb->SetCurrentState( D3D12_RESOURCE_STATE_GENERIC_READ );
	cb->SetResourceSize( bufferSize );

	ResourceID id = GenerateUniqueResourceId();
	RegisterPerFrameResource( id, std::move( cb ) );
	return id;
}

ResourceID GpuResourceManager::CreateTexture( const std::vector<D3D12_SUBRESOURCE_DATA>& subresourceData, D3D12_RESOURCE_DESC texDesc, const std::string& name )
{
	auto device = renderer->GetDevice();
	uint numSubresources = static_cast<uint>(subresourceData.size());

	// 1. Create the default heap resource (GPU local)
	CD3DX12_HEAP_PROPERTIES heapProps( D3D12_HEAP_TYPE_DEFAULT );
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	HRESULT hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS( &textureResource )
	);
	if( FAILED( hr ) ) return "resource not found";

	// 2. Create the upload heap
	UINT64 uploadBufferSize = 0;
	device->GetCopyableFootprints(
		&texDesc, 0, numSubresources, 0, nullptr, nullptr, nullptr, &uploadBufferSize );

	CD3DX12_HEAP_PROPERTIES uploadHeapProps( D3D12_HEAP_TYPE_UPLOAD );
	CD3DX12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer( uploadBufferSize );

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;
	hr = device->CreateCommittedResource(
		&uploadHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&uploadBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS( &uploadResource )
	);
	if( FAILED( hr ) ) return "resource not found";

	// 3. Create the Texture resource object
	auto tex = std::make_unique<Texture>( subresourceData, name );
	tex->SetResource( std::move( textureResource ) );
	tex->SetUploadResource( std::move( uploadResource ) );
	tex->SetResourceSize( uploadBufferSize );
	tex->SetCurrentState( D3D12_RESOURCE_STATE_COPY_DEST );

	ResourceID id = GenerateUniqueResourceId();
	RegisterResource( id, std::move( tex ) );

	return id;
}
