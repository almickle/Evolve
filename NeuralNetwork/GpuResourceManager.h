#pragma once
#include <atomic>
#include <combaseapi.h>
#include <cstdint>
#include <d3d12.h>
#include <d3dx12_core.h>
#include <DirectXTex.h>
#include <dxgiformat.h>
#include <memory>
#include <rpcndr.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>
#include <wrl\client.h>
#include "DataStructures.h"
#include "GpuResource.h"
#include "StructuredBuffer.h"
#include "System.h"
#include "SystemManager.h"
#include "Types.h"

class Renderer;
class DescriptorHeapManager;

class GpuResourceManager : public System {
public:
	GpuResourceManager( SystemManager& systemManager )
		: renderer( systemManager.GetRenderer() ),
		srvHeapManager( systemManager.GetSrvHeapManager() )
	{
	}
	~GpuResourceManager();
public:
	bool RegisterResource( ResourceID id, std::unique_ptr<GpuResource> resource );
	bool RegisterPerFrameResource( ResourceID id, std::unique_ptr<GpuResource> resource );
	void RemoveResource( const ResourceID& id );
	ResourceID GenerateUniqueResourceId( const std::string& prefix = "Resource" );
public:
	ResourceID CreateVertexBuffer( const std::vector<Vertex>& vertices, const std::string& name = "VertexBuffer" );
	ResourceID CreateIndexBuffer( const std::vector<uint>& vertices, const std::string& name = "IndexBuffer" );
	ResourceID CreateConstantBuffer( void* data, const std::string& name = "ConstantBuffer" );
	template<typename T>
	ResourceID CreateStaticStructuredBuffer( const std::vector<T>& data, const std::string& name = "StructuredBuffer" );
	template<typename T>
	ResourceID CreateDynamicStructuredBuffer( const std::vector<T>& data, const std::string& name );
	ResourceID CreateTexture( std::shared_ptr<DirectX::ScratchImage> image, const std::string& name = "Texture" );
public:
	GpuResource* GetResource( const ResourceID& id ) const;
	std::vector<GpuResource*> GetAllResources() const;
	std::vector<GpuResource*> GetCurrentFrameResources() const;
private:
	std::unordered_map<ResourceID, std::unique_ptr<GpuResource>> resourceHeap;
	std::unordered_map<ResourceID, std::vector<std::unique_ptr<GpuResource>>> perFrameResourceHeap;
	std::atomic<uint64_t> g_resourceIdCounter{ 0 };
private:
	Renderer* renderer;
	DescriptorHeapManager* srvHeapManager;
};

template<typename T>
ResourceID GpuResourceManager::CreateStaticStructuredBuffer( const std::vector<T>& data, const std::string& name )
{
	if( data.empty() ) return;

	auto device = renderer.GetDevice();
	uint elementSize = sizeof( T );
	uint elementCount = static_cast<uint>(data.size());
	uint64_t bufferSize = static_cast<uint64_t>(elementSize) * elementCount;

	// Create default heap (GPU local)
	CD3DX12_HEAP_PROPERTIES heapProps( D3D12_HEAP_TYPE_DEFAULT );
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );

	Microsoft::WRL::ComPtr<ID3D12Resource> gpuResource;
	HRESULT hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS( &gpuResource )
	);
	if( FAILED( hr ) ) return "resource not found";

	// Create upload heap
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

	// Create the StructuredBuffer resource
	auto sb = std::make_unique<StructuredBuffer<T>>( data, name );
	sb->SetResource( std::move( gpuResource ) );
	sb->SetCurrentState( D3D12_RESOURCE_STATE_COMMON );
	sb->SetUploadResource( std::move( uploadResource ) );
	sb->SetResourceSize( bufferSize );

	// Create SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = elementCount;
	srvDesc.Buffer.StructureByteStride = elementSize;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	sb->CreateSRV( *renderer, srvDesc );

	ResourceID id = GenerateUniqueResourceId();
	RegisterResource( id, std::move( sb ) );
	return id;
}

template<typename T>
ResourceID GpuResourceManager::CreateDynamicStructuredBuffer( const std::vector<T>& data, const std::string& name )
{
	if( data.empty() ) return;

	auto device = renderer.GetDevice();
	uint elementSize = sizeof( T );
	uint elementCount = static_cast<uint>(data.size());
	uint64_t bufferSize = static_cast<uint64_t>(elementSize) * elementCount;

	// Create default heap (GPU local)
	CD3DX12_HEAP_PROPERTIES heapProps( D3D12_HEAP_TYPE_DEFAULT );
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );

	Microsoft::WRL::ComPtr<ID3D12Resource> gpuResource;
	HRESULT hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS( &gpuResource )
	);
	if( FAILED( hr ) ) return "resource not found";

	// Create upload heap
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

	// Create the StructuredBuffer resource
	auto sb = std::make_unique<StructuredBuffer<T>>( data, name );
	sb->SetResource( std::move( gpuResource ) );
	sb->SetCurrentState( D3D12_RESOURCE_STATE_COMMON );
	sb->SetUploadResource( std::move( uploadResource ) );
	sb->SetResourceSize( bufferSize );

	// Create SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = elementCount;
	srvDesc.Buffer.StructureByteStride = elementSize;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	sb->CreateSRV( *renderer, srvDesc );

	ResourceID id = GenerateUniqueResourceId();
	RegisterPerFrameResource( id, std::move( sb ) );
	return id;
}