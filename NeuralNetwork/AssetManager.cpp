#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Asset.h"
#include "AssetManager.h"
#include "ImportManager.h"
#include "Material.h"
#include "MaterialTemplate.h"
#include "Mesh.h"
#include "Model.h"
#include "TextureAsset.h"
#include "Types.h"

void AssetManager::SaveAsset( const AssetID& id, const std::string& additionalPath ) const
{
	auto asset = GetAsset( id );
	auto content = asset->Serialize( serializer );
	asset->Save( assetDirectory / additionalPath, content );
}

void AssetManager::LoadAsset( const std::string& path, ImportManager& importManager )
{
	std::filesystem::path assetPath = assetDirectory / path;
	serializer.LoadFromFile( assetPath.string() );

	AssetType assetId = serializer.Read<AssetType>( "type" );

	switch( assetId )
	{
		case AssetType::Texture:
		{
			auto asset = std::make_unique<TextureAsset>( importManager );
			asset->Load( resourceManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::Mesh:
		{
			auto asset = std::make_unique<Mesh>();
			asset->Load( resourceManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::Model:
		{
			auto asset = std::make_unique<Model>();
			asset->Load( resourceManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::Material:
		{
			auto asset = std::make_unique<Material>();
			asset->Load( resourceManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::MaterialTemplate:
		{
			auto asset = std::make_unique<MaterialTemplate>( nodeLibrary );
			asset->Load( resourceManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
	}
}

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

void AssetManager::RegisterAsset( const AssetID& id, std::unique_ptr<Asset> asset )
{
	if( !asset ) return;

	// Check if pointer already exists in the registry
	for( const auto& pair : assetHeap ) {
		if( pair.second.get() == asset.get() ) {
			// Asset pointer already registered
			return;
		}
	}

	// Insert into the map
	assetHeap.emplace( id, std::move( asset ) );
}

void AssetManager::RemoveAsset( const AssetID& id )
{
	assetHeap.erase( id );
}

const Asset* AssetManager::GetAsset( const AssetID& id ) const
{
	auto it = assetHeap.find( id );
	return (it != assetHeap.end()) ? it->second.get() : nullptr;
}

Asset* AssetManager::GetAsset( const AssetID& id )
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
