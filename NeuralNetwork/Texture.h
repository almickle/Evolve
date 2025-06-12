#pragma once
#include <d3d12.h>
#include <string>
#include <vector>
#include "GpuResource.h"
#include "Types.h"

class Renderer;

class Texture : public GpuResource {
public:
	Texture( const std::vector<D3D12_SUBRESOURCE_DATA>& subresourceData, const std::string& name = "Texture" )
		: GpuResource( D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, name ),
		subresourceData( subresourceData ),
		numSubresources( static_cast<uint>(subresourceData.size()) )
	{
	};
	~Texture()
	{
		subresourceData.clear();
	}
public:
	void Upload( ID3D12GraphicsCommandList* cmdList ) override;
	const void* GetData() const override { return subresourceData.data(); }
	size_t GetDataSize() const override;
private:
	std::vector<D3D12_SUBRESOURCE_DATA> subresourceData;
	uint numSubresources = 0;
};