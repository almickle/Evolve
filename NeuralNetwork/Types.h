#pragma once
#include <string>

using uint = unsigned int;
using byte = unsigned char;

using ResourceID = std::string;
using AssetID = std::string;

enum class AssetType {
	Unknown,
	Texture,
	Mesh,
	Model,
	Material,
};

enum class SubAssetType {
	Unknown,
	SubMesh,
};