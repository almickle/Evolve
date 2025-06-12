#pragma once
#include <d3d12.h>
#include <memory>
#include <rpcndr.h>
#include <string>
#include <vector>
#include "GpuResource.h"

class Renderer;

class ConstantBuffer : public GpuResource {
public:
	ConstantBuffer( const std::vector<byte>& data, const std::string& name = "ConstantBuffer" )
		: GpuResource( D3D12_RESOURCE_STATE_GENERIC_READ, name ),
		bufferData( data )
	{
		bufferData.clear();
	}
	~ConstantBuffer() = default;
public:
	void Update( const void* data, size_t size ) override;
	void Upload( ID3D12GraphicsCommandList* cmdList ) override;
	std::unique_ptr<GpuResource> Clone( Renderer& renderer ) const override;
public:
	const void* GetData() const override { return bufferData.data(); }
	size_t GetDataSize() const override { return bufferData.size(); }
private:
	std::vector<byte> bufferData;
};