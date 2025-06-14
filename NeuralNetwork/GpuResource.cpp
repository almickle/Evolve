#include <cstring>
#include <d3d12.h>
#include <d3dx12_barriers.h>
#include <string>
#include <wrl\client.h>
#include "DescriptorHeapManager.h"
#include "GpuResource.h"
#include "Renderer.h"

void GpuResource::CreateSRV( DescriptorHeapManager& srvHeapManager, Renderer& renderer, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc )
{
	srvHeapIndex = srvHeapManager.Allocate();
	srvCpuHandle = srvHeapManager.GetCpuHandle( srvHeapIndex );
	srvGpuHandle = srvHeapManager.GetGpuHandle( srvHeapIndex );

	auto device = renderer.GetDevice();
	device->CreateShaderResourceView( resource.Get(), &srvDesc, srvCpuHandle );
}

void GpuResource::Upload( ID3D12GraphicsCommandList* cmdList )
{
	Transition( cmdList, D3D12_RESOURCE_STATE_COPY_DEST );

	void* mapped = nullptr;
	uploadResource->Map( 0, nullptr, &mapped );
	memcpy( mapped, GetData(), GetDataSize() );
	uploadResource->Unmap( 0, nullptr );

	cmdList->CopyBufferRegion(
		resource.Get(), 0,
		uploadResource.Get(), 0,
		GetDataSize()
	);
}

void GpuResource::Transition( ID3D12GraphicsCommandList* commandList, const D3D12_RESOURCE_STATES& requestedState )
{
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(),
		state.current,
		requestedState
	);
	commandList->ResourceBarrier( 1, &barrier );
	state.current = requestedState;
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
		resource->SetName( std::wstring( name.begin(), name.end() ).c_str() );
	}
}

void GpuResource::SetUploadResource( Microsoft::WRL::ComPtr<ID3D12Resource>&& res )
{
	uploadResource = res;
	if( uploadResource ) {
		std::wstring wname( name.begin(), name.end() );
		wname += L"_UploadResource";
		uploadResource->SetName( wname.c_str() );
	}
}