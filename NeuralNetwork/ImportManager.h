#pragma once
#include <d3d12.h>
#include <string>
#include <vector>
#include "AssetManager.h"
#include "GpuResourceManager.h"
#include "Mesh.h"

struct TextureData
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	D3D12_RESOURCE_DESC texDesc;
};

class ImportManager
{
public:
	ImportManager( AssetManager& assetManager, GpuResourceManager& resourceManager )
		: assetManager( assetManager ), resourceManager( resourceManager )
	{
	}
	~ImportManager() = default;
public:
	void ImportMesh( const std::string& path, const std::string& name );
	void ImportTexture( const std::string& path, const std::string& name );
	std::vector<MeshData> LoadMesh( const std::string& path );
	TextureData LoadTexture( const std::string& path );
private:
	AssetManager& assetManager;
	GpuResourceManager& resourceManager;
};

