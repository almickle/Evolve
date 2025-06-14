#pragma once
#include <string>

using uint = unsigned int;
using byte = unsigned char;

using ResourceID = std::string;
using AssetID = std::string;

enum class AssetType {
	Texture,
	Mesh,
	Model,
	Material,
	MaterialTemplate,
	Scene
};

enum class SubAssetType {
	SubMesh
};