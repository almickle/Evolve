//#pragma once
//#include <d3d12.h>
//#include <string>
//#include <Windows.h>
//#include "GpuResource.h"
//
//class Renderer;
//
//class Texture : public GpuResource {
//public:
//	Texture( const std::wstring& filePath );
//
//	// Loads the texture and creates the GPU resource and SRV
//	bool Load( Renderer& renderer );
//
//	// Separate creation and upload
//	bool CreateResource( Renderer& renderer, const D3D12_RESOURCE_DESC& texDesc, uint numSubresources );
//	void Upload( Renderer& renderer, const D3D12_SUBRESOURCE_DATA* subresources, uint numSubresources );
//
//	// Getters
//	D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHandle() const { return srvCpuHandle; }
//	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle() const { return srvGpuHandle; }
//	int GetSrvHeapIndex() const { return srvHeapIndex; }
//
//private:
//	std::wstring filePath;
//	D3D12_RESOURCE_STATES finalState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
//};