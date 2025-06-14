#pragma once
#include <d3d12.h>
#include <DirectXTex.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "GpuResource.h"
#include "Types.h"

class Texture : public GpuResource {
public:
	Texture( std::shared_ptr<DirectX::ScratchImage> image, const std::vector<D3D12_SUBRESOURCE_DATA>& subresourceData, const std::string& name = "Texture" )
		: GpuResource( D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, name ),
		image( std::move( image ) ),
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
	std::shared_ptr<DirectX::ScratchImage> image;
	std::vector<D3D12_SUBRESOURCE_DATA> subresourceData;
	uint numSubresources = 0;
};