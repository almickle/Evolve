#pragma once
#include <combaseapi.h>
#include <d3d12.h>
#include <d3dx12_core.h>
#include <dxgiformat.h>
#include <memory>
#include <string>
#include <vector>
#include <Windows.h>
#include <wrl\client.h>
#include "GpuResource.h"
#include "Renderer.h"
#include "Types.h"

template<typename T>
class StructuredBuffer : public GpuResource {
public:
	StructuredBuffer( const std::vector<T>& _data, const std::string& name = "StructuredBuffer" )
		: GpuResource( D3D12_RESOURCE_STATE_GENERIC_READ ),
		elementSize( sizeof( T ) ),
		elementCount( _data.size() ),
		data( _data )
	{
		size = (uint)data.size() * sizeof( T );
	}
	~StructuredBuffer()
	{
		data.clear();
	};
public:
	std::unique_ptr<GpuResource> Clone( DescriptorHeapManager& srvHeapManager, Renderer& renderer ) const override;
	uint GetElementSize() const { return elementSize; }
	uint GetElementCount() const { return elementCount; }
	void* GetData() override { return data.data(); }
private:
	uint elementSize = 0;
	uint elementCount = 0;
	std::vector<T> data;
};

template<typename T>
std::unique_ptr<GpuResource> StructuredBuffer<T>::Clone( DescriptorHeapManager& srvHeapManager, Renderer& renderer ) const
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

	auto clone = std::make_unique<StructuredBuffer<T>>( data, name );
	clone->resource = newResource;

	// Create SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = elementCount;
	srvDesc.Buffer.StructureByteStride = elementSize;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	clone->CreateSRV( srvHeapManager, renderer, srvDesc );
	// Copy other relevant metadata as needed
	return clone;
}