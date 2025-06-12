#pragma once
#include <string>
#include <vector>
#include "SubAsset.h"
#include "Types.h"

class Renderer;
class GpuResourceManager;

class SubMesh : public SubAsset {
public:
	SubMesh( ResourceID& vertexBufferID, ResourceID& indexBufferID, const std::string& name = "SubMesh" )
		: SubAsset( SubAssetType::SubMesh, name )
	{
		AddResource( vertexBufferID );
		AddResource( indexBufferID );
	}
	~SubMesh() = default;
public:
	ResourceID GetVertexBuffer() const { return resourceIDs[0]; };
	ResourceID GetIndexBuffer() const { return resourceIDs[1]; };
};