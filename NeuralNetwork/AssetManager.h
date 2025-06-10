#pragma once
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Asset.h"
#include "Types.h"

class AssetManager {
public:
	AssetManager() = default;
	~AssetManager() = default;
public:
	AssetID RegisterAsset( std::unique_ptr<Asset> asset );
	void RemoveAsset( const AssetID& id );
public:
	Asset* GetAsset( const AssetID& id );
	const Asset* GetAsset( const AssetID& id ) const;
	std::vector<Asset*> GetAllAssets() const;
private:
	AssetID GenerateUniqueAssetId();
private:
	std::unordered_map<AssetID, std::unique_ptr<Asset>> assetHeap;
	uint64_t assetIdCounter = 0;
};