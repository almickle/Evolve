#pragma once
#include <d3d12.h>
#include <memory>
#include <string>
#include "GpuResource.h"

class Renderer;

class ConstantBuffer : public GpuResource {
public:
	ConstantBuffer( void* data, const std::string& name = "ConstantBuffer" )
		: GpuResource( D3D12_RESOURCE_STATE_GENERIC_READ, name ),
		data( data )
	{
	}
	~ConstantBuffer() = default;
public:
	void Update( const void* data, size_t size ) override;
	void Upload( ID3D12GraphicsCommandList* cmdList ) override;
	std::unique_ptr<GpuResource> Clone( DescriptorHeapManager& srvHeapManager, Renderer& renderer ) const override;
public:
	void* GetData() const override { return data; }
	size_t GetDataSize() const override { return sizeof( data ); }
private:
	void* data;
};