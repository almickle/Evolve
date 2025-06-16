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
	Modifier,
	ModifierTemplate,
	Scene
};

enum class SubAssetType {
	SubMesh
};

enum class ShaderType {
	Pixel,
	Vertex,
	Domain,
	Hull,
	Compute
};