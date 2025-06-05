#include <combaseapi.h>
#include <d3d12.h>
#include <d3dx12_barriers.h>
#include <d3dx12_core.h>
#include <memory>
#include <string>
#include <utility>
#include <Windows.h>
#include <wrl\client.h>
#include "DescriptorHeapManager.h"
#include "GpuResource.h"
#include "Renderer.h"

void GpuResource::CreateSRV( Renderer& renderer, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc )
{
	auto* srvHeapManager = renderer.GetSrvHeapManager();
	srvHeapIndex = srvHeapManager->Allocate();
	srvCpuHandle = srvHeapManager->GetCpuHandle( srvHeapIndex );
	srvGpuHandle = srvHeapManager->GetGpuHandle( srvHeapIndex );

	auto device = renderer.GetDevice();
	device->CreateShaderResourceView( resource.Get(), &srvDesc, srvCpuHandle );
}

std::unique_ptr<GpuResource> GpuResource::Clone( Renderer& renderer ) const
{
	// Example for a buffer; adapt for textures as needed
	auto desc = resource->GetDesc();
	Microsoft::WRL::ComPtr<ID3D12Resource> newResource;
	CD3DX12_HEAP_PROPERTIES heapProps( D3D12_HEAP_TYPE_UPLOAD );
	HRESULT hr = renderer.GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		state.target,
		nullptr,
		IID_PPV_ARGS( &newResource )
	);
	if( FAILED( hr ) ) return nullptr;

	auto clone = std::make_unique<GpuResource>( state.target, debugName );
	clone->resource = newResource;
	clone->resourceSize = resourceSize;
	clone->debugName = debugName;
	// Copy other relevant metadata as needed
	return clone;
}

void GpuResource::TransitionToTargetState( ID3D12GraphicsCommandList* commandList )
{
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(),
		state.current,
		state.target
	);
	commandList->ResourceBarrier( 1, &barrier );
	state.current = state.target;
}

void GpuResource::SetResource( Microsoft::WRL::ComPtr<ID3D12Resource>&& res )
{
	resource = res;
	if( resource ) {
		resource->SetName( std::wstring( debugName.begin(), debugName.end() ).c_str() );
	}
}

void GpuResource::SetUploadResource( Microsoft::WRL::ComPtr<ID3D12Resource>&& res )
{
	uploadResource = res;
	if( uploadResource ) {
		std::wstring wname( debugName.begin(), debugName.end() );
		wname += L"_UploadResource";
		uploadResource->SetName( wname.c_str() );
	}
}