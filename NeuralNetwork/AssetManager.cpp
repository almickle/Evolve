#pragma once
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Asset.h"
#include "AssetManager.h"
#include "Types.h"

// Register an asset, assign a unique ID, and check for duplicate pointers
AssetID AssetManager::RegisterAsset( std::unique_ptr<Asset> asset )
{
	if( !asset ) return {};

	// Check if pointer already exists in the registry
	for( const auto& pair : assetHeap ) {
		if( pair.second.get() == asset.get() ) {
			// Asset pointer already registered
			return {};
		}
	}

	// Assign a unique ID
	AssetID newId = GenerateUniqueAssetId();
	asset->SetAssetID( newId );

	// Insert into the map
	assetHeap.emplace( newId, std::move( asset ) );
	return newId;
}

void AssetManager::RemoveAsset( const AssetID& id )
{
	assetHeap.erase( id );
}

Asset* AssetManager::GetAsset( const AssetID& id )
{
	auto it = assetHeap.find( id );
	return (it != assetHeap.end()) ? it->second.get() : nullptr;
}
const Asset* AssetManager::GetAsset( const AssetID& id ) const
{
	auto it = assetHeap.find( id );
	return (it != assetHeap.end()) ? it->second.get() : nullptr;
}

std::vector<Asset*> AssetManager::GetAllAssets() const
{
	std::vector<Asset*> all;
	for( const auto& pair : assetHeap ) {
		all.push_back( pair.second.get() );
	}
	return all;
}

AssetID AssetManager::GenerateUniqueAssetId()
{
	return "Asset_" + std::to_string( assetIdCounter++ );
}
