#pragma once
#include <atomic>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "SubAsset.h"
#include "Types.h"

class GpuResourceManager;

class Asset {
public:
	Asset( AssetType assetType = AssetType::Unknown, const std::string& debugName = "Asset" )
		: assetType( assetType ), debugName( debugName )
	{
	}
	virtual ~Asset() = default;
public:
	virtual void Load( const std::string& filePath, GpuResourceManager& resourceManager ) = 0;
	virtual AssetType GetType() const { return assetType; }
public:
	std::vector<AssetID> GetAllAssetIDs() const { return assetIds; }
	std::vector<ResourceID> GetAllResourceIDs() const;
	bool IsReady() const { return ready.load( std::memory_order_acquire ); }
	void AddAsset( const ResourceID& id ) { assetIds.push_back( id ); }
	void AddSubAsset( std::unique_ptr<SubAsset> subasset ) { subassets.push_back( std::move( subasset ) ); }
	void AddResource( const ResourceID& id ) { resourceIDs.push_back( id ); }
public:
	void SetAssetID( AssetID& assetId ) { id = assetId; }
	void SetIsReady( bool val ) { ready.store( val, std::memory_order_release ); }
protected:
	AssetType assetType;
	AssetID id;
	std::string debugName;
	std::string filePath;
	std::vector<AssetID> assetIds;
	std::vector<ResourceID> resourceIDs;
	std::vector<std::unique_ptr<SubAsset>> subassets;
	std::atomic<bool> ready{ false };
};