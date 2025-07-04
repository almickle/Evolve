#pragma once
#include <atomic>
#include <d3d12.h>
#include <memory>
#include <string>
#include <wrl\client.h>

class Renderer;
class DescriptorHeapManager;

struct ResourceState {
	D3D12_RESOURCE_STATES current = D3D12_RESOURCE_STATE_COMMON;
	D3D12_RESOURCE_STATES target = D3D12_RESOURCE_STATE_COMMON;
	std::atomic<bool> ready{ false };
};

class GpuResource {
	using ResourceID = std::string;
public:
	GpuResource(
		D3D12_RESOURCE_STATES finalState,
		const std::string& name = "default" )
		: state( finalState ), name( name )
	{
	}
	virtual ~GpuResource() = default;
public:
	virtual std::unique_ptr<GpuResource> Clone( DescriptorHeapManager& srvHeapManager, Renderer& renderer ) const { return nullptr; }
	virtual void Update( const void* data, size_t size ) {}
	virtual void Upload( ID3D12GraphicsCommandList* cmdList );
	virtual void* GetData() { return nullptr; }
	virtual size_t GetDataSize() const { return size; }
public:
	void Transition( ID3D12GraphicsCommandList* commandList, const D3D12_RESOURCE_STATES& requestedState );
	void TransitionToTargetState( ID3D12GraphicsCommandList* commandList );
public:
	ID3D12Resource* GetResource() const { return resource.Get(); }
	D3D12_RESOURCE_DESC GetDesc() const { return resource->GetDesc(); }
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress() const { return resource->GetGPUVirtualAddress(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHandle() const { return srvCpuHandle; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle() const { return srvGpuHandle; }
	int GetSrvHeapIndex() const { return srvHeapIndex; }
	D3D12_RESOURCE_STATES GetCurrentState() const { return state.current; }
	D3D12_RESOURCE_STATES GetTargetState() const { return state.target; }
	bool IsStateTransitionNeeded() const { return state.current != state.target; }
public:
	void SetCurrentState( D3D12_RESOURCE_STATES newState ) { state.current = newState; }
	void SetResourceId( const ResourceID& id ) { resourceId = id; }
	void SetUploadResourceId( const ResourceID& id ) { uploadResourceId = id; }
	void SetResourceSize( size_t resourceSize ) { size = resourceSize; }
	void SetResource( Microsoft::WRL::ComPtr<ID3D12Resource>&& res );
	void SetUploadResource( Microsoft::WRL::ComPtr<ID3D12Resource>&& heap );
	void SetIsReady( bool ready ) { state.ready.store( ready, std::memory_order_release ); }
public:
	void CreateSRV( DescriptorHeapManager& srvHeapManager, Renderer& renderer, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, bool reserved = false );
	bool IsReady() const { return state.ready.load( std::memory_order_acquire ); }
protected:
	ResourceState state;
	ResourceID resourceId;
	ResourceID uploadResourceId;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;
	int srvHeapIndex = -1;
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = {};
	std::string name;
	size_t size = 0;
};