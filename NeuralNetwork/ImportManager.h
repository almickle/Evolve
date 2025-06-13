#pragma once
#include <d3d12.h>
#include <string>
#include <vector>
#include "Mesh.h"
#include "System.h"
#include "SystemManager.h"

class AssetManager;
class GpuResourceManager;

struct TextureData
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	D3D12_RESOURCE_DESC texDesc;
};

class ImportManager : public System
{
public:
	ImportManager( SystemManager& systemManager )
	{
	}
	~ImportManager() = default;
public:
	std::vector<MeshData> LoadMesh( const std::string& path );
	TextureData LoadTexture( const std::string& path );
};

