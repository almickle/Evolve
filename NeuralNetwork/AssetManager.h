#pragma once
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "Asset.h"
#include "FileIOManager.h"
#include "GpuResourceManager.h"
#include "JsonSerializer.h"
#include "NodeLibrary.h"
#include "Types.h"

class ImportManager;

class AssetManager {
public:
	AssetManager( JsonSerializer& serializer, FileIOManager& fileManager, GpuResourceManager& resourceManager, NodeLibrary& nodeLibrary )
		: serializer( serializer ),
		fileManager( fileManager ),
		resourceManager( resourceManager ),
		nodeLibrary( nodeLibrary )
	{
	};
	~AssetManager() = default;
public:
	AssetID RegisterAsset( std::unique_ptr<Asset> asset );
	void RegisterAsset( const AssetID& assetId, std::unique_ptr<Asset> asset );
	void RemoveAsset( const AssetID& id );
	void SaveAsset( const AssetID& id, const std::string& additionalPath = "" ) const;
	void LoadAsset( const std::string& name, ImportManager& importManager );
public:
	Asset* GetAsset( const AssetID& id );
	const Asset* GetAsset( const AssetID& id ) const;
	std::vector<Asset*> GetAllAssets() const;
private:
	AssetID GenerateUniqueAssetId();
private:
	std::unordered_map<AssetID, std::unique_ptr<Asset>> assetHeap;
	uint64_t assetIdCounter = 0;
	std::filesystem::path assetDirectory = std::filesystem::path( "Assets" );
private:
	JsonSerializer& serializer;
	FileIOManager& fileManager;
	GpuResourceManager& resourceManager;
	NodeLibrary& nodeLibrary;
};