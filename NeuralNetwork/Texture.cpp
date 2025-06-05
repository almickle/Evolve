//#include <algorithm>
//#include <cassert>
//#include <cctype>
//#include <combaseapi.h>
//#include <cstring>
//#include <d3d12.h>
//#include <d3dx12_barriers.h>
//#include <d3dx12_core.h>
//#include <d3dx12_resource_helpers.h>
//#include <DirectXTex.h>
//#include <DirectXTex.inl>
//#include <string>
//#include <Windows.h>
//#include "GpuResource.h"
//#include "Renderer.h"
//#include "Texture.h"
//#include "UploadManager.h"
//
//namespace {
//	enum class ImageType {
//		DDS, TGA, HDR, WIC, UNKNOWN
//	};
//
//	ImageType GetImageTypeFromExtension( const std::wstring& filePath )
//	{
//		auto pos = filePath.find_last_of( L'.' );
//		if( pos == std::wstring::npos ) return ImageType::UNKNOWN;
//		std::wstring ext = filePath.substr( pos + 1 );
//		std::transform( ext.begin(), ext.end(), ext.begin(), ::towlower );
//
//		if( ext == L"dds" ) return ImageType::DDS;
//		if( ext == L"tga" ) return ImageType::TGA;
//		if( ext == L"hdr" ) return ImageType::HDR;
//		// Add more as needed
//		return ImageType::WIC; // Default to WIC for common formats
//	}
//}
//
//Texture::Texture( const std::wstring& filePath )
//	: GpuResource( D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE ), filePath( filePath )
//{
//}
//
//bool Texture::Load( Renderer& renderer )
//{
//	// 1. Load the image using DirectXTex based on file extension
//	DirectX::ScratchImage image;
//	HRESULT hr = S_OK;
//
//	ImageType type = GetImageTypeFromExtension( filePath );
//
//	switch( type ) {
//		case ImageType::DDS:
//			hr = DirectX::LoadFromDDSFile(
//				filePath.c_str(),
//				DirectX::DDS_FLAGS_NONE,
//				nullptr,
//				image
//			);
//			break;
//		case ImageType::TGA:
//			hr = DirectX::LoadFromTGAFile(
//				filePath.c_str(),
//				nullptr,
//				image
//			);
//			break;
//		case ImageType::HDR:
//			hr = DirectX::LoadFromHDRFile(
//				filePath.c_str(),
//				nullptr,
//				image
//			);
//			break;
//		case ImageType::WIC:
//		default:
//			hr = DirectX::LoadFromWICFile(
//				filePath.c_str(),
//				DirectX::WIC_FLAGS_FORCE_RGB,
//				nullptr,
//				image
//			);
//			break;
//	}
//
//	if( FAILED( hr ) ) {
//		return false;
//	}
//
//	const DirectX::Image* img = image.GetImage( 0, 0, 0 );
//	assert( img );
//
//	// 2. Describe the texture
//	D3D12_RESOURCE_DESC texDesc = {};
//	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//	texDesc.Width = img->width;
//	texDesc.Height = static_cast<uint>(img->height);
//	texDesc.DepthOrArraySize = 1;
//	texDesc.MipLevels = 1;
//	texDesc.Format = img->format;
//	texDesc.SampleDesc.Count = 1;
//	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
//	texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
//
//	// 3. Create the resource
//	if( !CreateResource( renderer, texDesc, 1 ) )
//		return false;
//
//	// 4. Prepare subresource data
//	D3D12_SUBRESOURCE_DATA subresourceData = {};
//	subresourceData.pData = img->pixels;
//	subresourceData.RowPitch = img->rowPitch;
//	subresourceData.SlicePitch = img->slicePitch;
//
//	// 5. Upload the texture
//	Upload( renderer, &subresourceData, 1 );
//
//	// 6. Create SRV 
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.Format = texDesc.Format;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MipLevels = 1;
//	CreateSRV( renderer, srvDesc );
//
//	return true;
//}
//
//bool Texture::CreateResource( Renderer& renderer, const D3D12_RESOURCE_DESC& texDesc, uint numSubresources )
//{
//	auto device = renderer.GetDevice();
//
//	// 1. Create the default heap resource in COMMON state
//	CD3DX12_HEAP_PROPERTIES heapProps( D3D12_HEAP_TYPE_DEFAULT );
//	HRESULT hr = device->CreateCommittedResource(
//		&heapProps,
//		D3D12_HEAP_FLAG_NONE,
//		&texDesc,
//		D3D12_RESOURCE_STATE_COMMON,
//		nullptr,
//		IID_PPV_ARGS( &resource )
//	);
//	if( FAILED( hr ) ) return false;
//
//	// 2. Create the upload heap
//	uint64 uploadBufferSize = 0;
//	device->GetCopyableFootprints( &texDesc, 0, numSubresources, 0, nullptr, nullptr, nullptr, &uploadBufferSize );
//
//	CD3DX12_HEAP_PROPERTIES uploadHeapProps( D3D12_HEAP_TYPE_UPLOAD );
//	CD3DX12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer( uploadBufferSize );
//
//	hr = device->CreateCommittedResource(
//		&uploadHeapProps,
//		D3D12_HEAP_FLAG_NONE,
//		&uploadBufferDesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS( &uploadHeap )
//	);
//	return SUCCEEDED( hr );
//}
//
//void Texture::Upload( Renderer& renderer, const D3D12_SUBRESOURCE_DATA* subresources, uint numSubresources )
//{
//	// Copy data to upload heap
//	void* mapped = nullptr;
//	uploadHeap->Map( 0, nullptr, &mapped );
//	memcpy( mapped, subresources[0].pData, subresources[0].SlicePitch );
//	uploadHeap->Unmap( 0, nullptr );
//
//	auto* uploadManager = renderer.GetUploadManager();
//	if( uploadManager ) {
//		uploadManager->Enqueue( {
//			[this, numSubresources, subresources]( ID3D12GraphicsCommandList* cmdList ) {
//				CD3DX12_RESOURCE_BARRIER toCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
//					resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST );
//				cmdList->ResourceBarrier( 1, &toCopyDest );
//
//				UpdateSubresources( cmdList, resource.Get(), uploadHeap.Get(), 0, 0, numSubresources, subresources );
//
//				CD3DX12_RESOURCE_BARRIER toFinalState = CD3DX12_RESOURCE_BARRIER::Transition(
//					resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, finalState );
//				cmdList->ResourceBarrier( 1, &toFinalState );
//			},
//			[this]() { ready.store( true, std::memory_order_release ); }
//								} );
//	}
//	else {
//		assert( false && "UploadManager required for async upload" );
//	}
//}