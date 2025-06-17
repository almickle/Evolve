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

class Renderer;
class JsonSerializer;
class TaskManager;
class GpuResourceManager;
class NodeLibrary;
class ImportManager;

class AssetManager : public System {
	enum class LoadState { NotLoaded, Loading, Loaded };
public:
	AssetManager( SystemManager& systemManager )
		: renderer( systemManager.GetRenderer() ),
		serializer( systemManager.GetSerializer() ),
		taskManager( systemManager.GetTaskManager() ),
		resourceManager( systemManager.GetResourceManager() ),
		nodeLibrary( systemManager.GetNodeLibrary() ),
		importManager( systemManager.GetImportManager() )
	{
	};
	~AssetManager() = default;
public:
	void Init( SystemManager* systemManager );
public:
	AssetID RegisterAsset( std::unique_ptr<Asset> asset );
	void RegisterAsset( const AssetID& assetId, std::unique_ptr<Asset> asset );
	void RemoveAsset( const AssetID& id );
	void SaveAsset( const AssetID& id, const std::string& additionalPath = "" ) const;
	void LoadAsset( const std::string& path, SystemManager* systemManager, JsonSerializer& serializer );
public:
	void ImportMesh( const std::string& path, const std::string& name );
	void ImportTexture( const std::string& path, const std::string& name );
public:
	Asset* GetAsset( const AssetID& id );
	const Asset* GetAsset( const AssetID& id ) const;
	const std::vector<Asset*> GetAllAssets() const;
	std::vector<Asset*> GetAllAssets();
private:
	AssetID GenerateUniqueAssetId();
	void PostLoadProcessing();
private:
	std::unordered_map<AssetID, std::unique_ptr<Asset>> assetHeap;
	mutable std::mutex assetHeapMutex;
	std::unordered_map<AssetID, LoadState> assetLoadState;
	uint64_t assetIdCounter = 0;
	std::filesystem::path assetDirectory = std::filesystem::path( "Assets" );
	std::string assetFileExtension = ".json";
private:
	Renderer* renderer;
	JsonSerializer* serializer;
	TaskManager* taskManager;
	GpuResourceManager* resourceManager;
	NodeLibrary* nodeLibrary;
	ImportManager* importManager;
};