#pragma once
#include <d3d12.h>
#include <string>
#include "GpuResource.h"

class Renderer;

class Texture : public GpuResource {
public:
	Texture( const std::wstring& filePath, const std::string& debugName = "Texture" )
		: GpuResource( D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, debugName ),
		filePath( filePath )
	{
	};
public:
	bool Load();
private:
	std::wstring filePath;
	D3D12_RESOURCE_STATES finalState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
};