#pragma once
#include <atomic>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "JsonSerializer.h"
#include "SubAsset.h"
#include "Types.h"

class SystemManager;
class AssetManager;
class GpuResourceManager;
class DescriptorHeapManager;

class Asset {
public:
	Asset( AssetType assetType, const std::string& name = "Asset" )
		: assetType( assetType ), name( name )
	{
	}
	virtual ~Asset() = default;
public:
	virtual void Load( SystemManager* systemManager, JsonSerializer& serializer ) = 0;
	virtual std::string Serialize( JsonSerializer& serializer ) const = 0;
	virtual void Deserialize( JsonSerializer& serializer ) = 0;
	virtual void Update( AssetManager* assetManager, GpuResourceManager* resourceManager, DescriptorHeapManager* srvHeapManager ) {};
	void Save( const std::filesystem::path& dirPath, const std::string& content ) const;
public:
	AssetID GetAssetID() const { return id; }
	AssetType GetType() const { return assetType; }
	std::string GetName() const { return name; }
	std::vector<AssetID> GetAllAssetIDs() const { return assetIds; }
	std::vector<ResourceID> GetAllResourceIDs() const;
	bool IsReady() const { return ready.load( std::memory_order_acquire ); }
	bool IsDirty() const { return isDirty; }
	void AddAsset( const AssetID& id ) { assetIds.push_back( id ); }
	void AddSubAsset( std::unique_ptr<SubAsset> subasset ) { subassets.push_back( std::move( subasset ) ); }
	void AddResource( const ResourceID& id ) { resourceIDs.push_back( id ); }
public:
	void SetAssetID( const AssetID& assetId ) { id = assetId; }
	void SetIsReady( bool val ) { ready.store( val, std::memory_order_release ); }
	void SetIsDirty( bool val ) { isDirty = val; }
protected:
	void SerializeBaseAsset( JsonSerializer& serializer ) const;
	void DeserializeBaseAsset( JsonSerializer& serializer );
protected:
	AssetType assetType;
	AssetID id;
	std::string name;
	std::vector<AssetID> assetIds;
	std::vector<ResourceID> resourceIDs;
	std::vector<std::unique_ptr<SubAsset>> subassets;
	std::atomic<bool> ready{ false };
	bool isDirty{ false };
};