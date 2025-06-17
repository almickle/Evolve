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
		size_t total = 0;
		for( const auto& sub : subresourceData ) {
			total += sub.SlicePitch;
		}
		size = total;
	};
	~Texture()
	{
		subresourceData.clear();
	}
public:
	void Upload( ID3D12GraphicsCommandList* cmdList ) override;
	void* GetData() override
	{
		if( image && image->GetImages() && image->GetImageCount() > 0 )
			return const_cast<void*>(static_cast<const void*>(image->GetImages()->pixels));
		return nullptr;
	};
private:
	std::shared_ptr<DirectX::ScratchImage> image;
	std::vector<D3D12_SUBRESOURCE_DATA> subresourceData;
	uint numSubresources = 0;
};