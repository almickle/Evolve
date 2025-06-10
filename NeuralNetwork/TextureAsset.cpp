#include <algorithm>
#include <cctype>
#include <d3d12.h>
#include <DirectXTex.h>
#include <DirectXTex.inl>
#include <string>
#include <vector>
#include <Windows.h>
#include "GpuResourceManager.h"
#include "TextureAsset.h"

namespace {
	enum class ImageType {
		DDS, TGA, HDR, WIC, UNKNOWN
	};

	ImageType GetImageTypeFromExtension( const std::wstring& filePath )
	{
		auto pos = filePath.find_last_of( L'.' );
		if( pos == std::wstring::npos ) return ImageType::UNKNOWN;
		std::wstring ext = filePath.substr( pos + 1 );
		std::transform( ext.begin(), ext.end(), ext.begin(), ::towlower );

		if( ext == L"dds" ) return ImageType::DDS;
		if( ext == L"tga" ) return ImageType::TGA;
		if( ext == L"hdr" ) return ImageType::HDR;
		// Add more as needed
		return ImageType::WIC; // Default to WIC for common formats
	}
}

void TextureAsset::Load( const std::string& path, GpuResourceManager& manager )
{
	// 1. Load the image using DirectXTex based on file extension
	DirectX::ScratchImage image;
	HRESULT hr = S_OK;
	std::wstring filePath( path.begin(), path.end() );

	ImageType type = GetImageTypeFromExtension( filePath );

	switch( type ) {
		case ImageType::DDS:
			hr = DirectX::LoadFromDDSFile(
				filePath.c_str(),
				DirectX::DDS_FLAGS_NONE,
				nullptr,
				image
			);
			break;
		case ImageType::TGA:
			hr = DirectX::LoadFromTGAFile(
				filePath.c_str(),
				nullptr,
				image
			);
			break;
		case ImageType::HDR:
			hr = DirectX::LoadFromHDRFile(
				filePath.c_str(),
				nullptr,
				image
			);
			break;
		case ImageType::WIC:
		default:
			hr = DirectX::LoadFromWICFile(
				filePath.c_str(),
				DirectX::WIC_FLAGS_FORCE_RGB,
				nullptr,
				image
			);
			break;
	}

	// 2. Describe the texture
	const DirectX::TexMetadata& metadata = image.GetMetadata();
	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	texDesc.Width = static_cast<UINT>(metadata.width);
	texDesc.Height = static_cast<UINT>(metadata.height);
	texDesc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
	texDesc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
	texDesc.Format = metadata.format;
	texDesc.SampleDesc.Count = 1;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// 3. Prepare subresource data for all subresources
	size_t numSubresources = image.GetImageCount();
	std::vector<D3D12_SUBRESOURCE_DATA> subresources( numSubresources );
	const DirectX::Image* images = image.GetImages();
	for( size_t i = 0; i < numSubresources; ++i ) {
		subresources[i].pData = images[i].pixels;
		subresources[i].RowPitch = images[i].rowPitch;
		subresources[i].SlicePitch = images[i].slicePitch;
	}

	auto id = manager.CreateTexture( subresources, texDesc, debugName );
	AddResource( id );
}