#pragma once
#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "Asset.h"
#include "System.h"
#include "SystemManager.h"
#include "Types.h"

class JsonSerializer;
class FileIOManager;
class GpuResourceManager;
class NodeLibrary;
class ImportManager;

class AssetManager : public System {
public:
	AssetManager( SystemManager& systemManager )
		: serializer( serializer ),
		fileManager( systemManager.GetFileManager() ),
		resourceManager( systemManager.GetResourceManager() ),
		nodeLibrary( systemManager.GetNodeLibrary() ),
		importManager( systemManager.GetImportManager() )
	{
	};
	~AssetManager() = default;
public:
	void Init();
	AssetID RegisterAsset( std::unique_ptr<Asset> asset );
	void RegisterAsset( const AssetID& assetId, std::unique_ptr<Asset> asset );
	void RemoveAsset( const AssetID& id );
	void SaveAsset( const AssetID& id, const std::string& additionalPath = "" ) const;
	void LoadAsset( const std::string& name );
public:
	void ImportMesh( const std::string& path, const std::string& name );
	void ImportTexture( const std::string& path, const std::string& name );
public:
	Asset* GetAsset( const AssetID& id );
	const Asset* GetAsset( const AssetID& id ) const;
	std::vector<Asset*> GetAllAssets() const;
private:
	AssetID GenerateUniqueAssetId();
private:
	std::unordered_map<AssetID, std::unique_ptr<Asset>> assetHeap;
	mutable std::mutex assetHeapMutex;
	uint64_t assetIdCounter = 0;
	std::filesystem::path assetDirectory = std::filesystem::path( "Assets" );
	std::string assetFileExtension = ".json";
private:
	JsonSerializer* serializer;
	FileIOManager* fileManager;
	GpuResourceManager* resourceManager;
	NodeLibrary* nodeLibrary;
	ImportManager* importManager;
};