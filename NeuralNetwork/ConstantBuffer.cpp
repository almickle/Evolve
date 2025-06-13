#include <combaseapi.h>
#include <cstring>
#include <d3d12.h>
#include <d3dx12_core.h>
#include <memory>
#include <Windows.h>
#include <wrl\client.h>
#include "ConstantBuffer.h"
#include "GpuResource.h"
#include "Renderer.h"

void ConstantBuffer::Update( const void* data, size_t size )
{
	if( size == bufferData.size() ) {
		std::memcpy( bufferData.data(), data, size );
	}
	// Optionally handle size mismatch or reallocation
}

void ConstantBuffer::Upload( ID3D12GraphicsCommandList* cmdList )
{
	void* mapped = nullptr;
	uploadResource->Map( 0, nullptr, &mapped );
	memcpy( mapped, GetData(), GetDataSize() );
	uploadResource->Unmap( 0, nullptr );
}

std::unique_ptr<GpuResource> ConstantBuffer::Clone( DescriptorHeapManager& srvHeapManager, Renderer& renderer ) const
{
	// Example for a buffer; adapt for textures as needed
	auto desc = resource->GetDesc();
	Microsoft::WRL::ComPtr<ID3D12Resource> newResource;
	CD3DX12_HEAP_PROPERTIES heapProps( D3D12_HEAP_TYPE_UPLOAD );
	HRESULT hr = renderer.GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		state.current,
		nullptr,
		IID_PPV_ARGS( &newResource )
	);
	if( FAILED( hr ) ) return nullptr;

	auto clone = std::make_unique<ConstantBuffer>( bufferData, name );
	clone->resource = newResource;
	clone->resourceSize = resourceSize;
	clone->name = name;
	// Copy other relevant metadata as needed
	return clone;
}