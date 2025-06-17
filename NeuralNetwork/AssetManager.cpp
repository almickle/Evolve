#pragma once
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Asset.h"
#include "AssetManager.h"
#include "ImportManager.h"
#include "JsonSerializer.h"
#include "Material.h"
#include "MaterialTemplate.h"
#include "Mesh.h"
#include "Model.h"
#include "Modifier.h"
#include "ModifierTemplate.h"
#include "Renderer.h"
#include "Scene.h"
#include "SubMesh.h"
#include "SystemManager.h"
#include "TaskManager.h"
#include "TextureAsset.h"
#include "Types.h"

void AssetManager::SaveAsset( const AssetID& id, const std::string& additionalPath ) const
{
	auto asset = GetAsset( id );
	auto content = asset->Serialize( *serializer );
	asset->Save( assetDirectory / additionalPath, content );
}

void AssetManager::Init( SystemManager* systemManager )
{
	namespace fs = std::filesystem;

	// 1. Collect all asset file paths
	std::vector<std::string> assetPaths;
	for( const auto& entry : fs::recursive_directory_iterator( assetDirectory, fs::directory_options::skip_permission_denied ) ) {
		if( entry.is_regular_file() && entry.path().extension() == assetFileExtension ) {
			assetPaths.push_back( fs::absolute( entry.path() ).string() );
		}
	}

	// 2. Read dependencies for each asset file
	std::unordered_map<std::string, AssetID> pathToId;
	std::unordered_map<std::string, std::vector<AssetID>> pathToDeps;

	for( const auto& path : assetPaths ) {
		JsonSerializer serializer;
		serializer.LoadFromFile( path );

		// Read this asset's ID
		AssetID id = serializer.Read<AssetID>( "id" );
		pathToId[path] = id;

		// Read dependencies (if present)
		std::vector<AssetID> deps;
		if( serializer.ReadArray<AssetID>( "assetIds" ).size() > 0 ) {
			deps = serializer.ReadArray<AssetID>( "assetIds" );
		}
		pathToDeps[path] = deps;
	}

	// 3. Build dependency graph (by file path)
	std::unordered_map<AssetID, std::string> idToPath;
	for( const auto& [path, id] : pathToId ) {
		idToPath[id] = path;
	}

	std::unordered_map<AssetID, std::vector<AssetID>> idDeps;
	for( const auto& [path, deps] : pathToDeps ) {
		AssetID id = pathToId[path];
		idDeps[id] = deps;
	}

	// 4. Initialize load state
	std::unordered_map<AssetID, LoadState> assetLoadState;
	for( const auto& [id, _] : idDeps ) {
		assetLoadState[id] = LoadState::NotLoaded;
	}

	// 5. Lambda to check if all dependencies are loaded
	auto canLoad = [&]( const AssetID& id ) {
		for( const auto& dep : idDeps[id] ) {
			if( assetLoadState[dep] != LoadState::Loaded )
				return false;
		}
		return true;
		};

	// 6. Assets waiting for dependencies
	std::unordered_map<AssetID, std::vector<AssetID>> dependents;
	for( const auto& [id, deps] : idDeps ) {
		for( const auto& dep : deps ) {
			dependents[dep].push_back( id );
		}
	}

	// 7. Queue for assets ready to load
	std::queue<AssetID> ready;
	std::mutex readyMutex;

	for( const auto& [id, _] : idDeps ) {
		if( canLoad( id ) ) {
			ready.push( id );
			assetLoadState[id] = LoadState::Loading;
		}
	}

	// 8. Task completion callback
	std::function<void( const AssetID& )> onAssetLoaded;
	onAssetLoaded = [&]( const AssetID& id ) {
		assetLoadState[id] = LoadState::Loaded;
		for( const auto& dependent : dependents[id] ) {
			if( assetLoadState[dependent] == LoadState::NotLoaded && canLoad( dependent ) ) {
				{
					std::lock_guard<std::mutex> lock( readyMutex );
					ready.push( dependent );
				}
				assetLoadState[dependent] = LoadState::Loading;
			}
		}
		};

	// 9. Main asset loading loop
	size_t totalAssets = assetLoadState.size();
	while( true ) {
		AssetID id;
		{
			std::lock_guard<std::mutex> lock( readyMutex );
			if( ready.empty() ) {
				// Check if all assets are loaded
				size_t loadedCount = 0;
				for( const auto& [_, state] : assetLoadState ) {
					if( state == LoadState::Loaded )
						++loadedCount;
				}
				if( loadedCount == totalAssets )
					break; // All assets are loaded, safe to exit loop

				// Otherwise, wait for more assets to become ready
				std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
				continue;
			}
			id = ready.front();
			ready.pop();
		}
		const std::string& path = idToPath[id];
		JsonSerializer serializer;
		Task task;
		task.path = path;
		task.taskFunc = [this, systemManager]( const std::string& filePath ) {
			JsonSerializer localSerializer;
			this->LoadAsset( filePath, systemManager, localSerializer );
			};
		task.onComplete = [id, &onAssetLoaded]() {
			onAssetLoaded( id );
			};
		taskManager->Enqueue( std::move( task ) );
	}

	auto fence = taskManager->InsertFence();
	fence.wait();

	// 10. Post-load processing (e.g., shader compilation, PSO creation)
	PostLoadProcessing();

}

void AssetManager::PostLoadProcessing()
{
	for( const auto& asset : GetAllAssets() )
	{
		auto type = asset->GetType();
		switch( type )
		{
			case AssetType::MaterialTemplate:
			{
				auto* assetRef = static_cast<MaterialTemplate*>(asset);
				assetRef->GenerateShaderCode();
				renderer->CompileShader( assetRef->GetShaderCode(), ShaderType::Pixel, assetRef->GetName(), assetRef->GetPixelShaderBlob() );
			}
			break;
			case AssetType::ModifierTemplate:
			{
				auto* assetRef = static_cast<ModifierTemplate*>(asset);
				assetRef->GenerateShaderCode();
				renderer->CompileShader( assetRef->GetShaderCode(), ShaderType::Vertex, assetRef->GetName(), assetRef->GetVertexShaderBlob() );
			}
			break;
		}
	}

	for( const auto& asset : GetAllAssets() )
	{
		if( asset->GetType() == AssetType::Model )
		{
			auto* model = static_cast<Model*>(asset);
			for( uint i = 0; i < model->GetMaterialSlots().size(); i++ )
			{
				auto* material = static_cast<Material*>( GetAsset( model->GetMaterialSlots()[i] ) );
				auto* matTemplate = static_cast<MaterialTemplate*>( GetAsset( material->GetMaterialTemplate() ) );
				auto* modifier = static_cast<Modifier*>( GetAsset( model->GetModifierSlots()[i] ) );
				auto* modTemplate = static_cast<ModifierTemplate*>( GetAsset( modifier->GetModifierTemplate() ) );

				auto psoKey = renderer->CreatePipelineState( modTemplate->GetVertexShaderBlob(), matTemplate->GetPixelShaderBlob(), modTemplate->GetDomainShaderBlob(), modTemplate->GetHullShaderBlob() );
				model->AddPsoKey( psoKey );
			}
		}
	}
}

void AssetManager::LoadAsset( const std::string& path, SystemManager* systemManager, JsonSerializer& serializer )
{
	std::filesystem::path assetPath = assetDirectory / path;
	serializer.LoadFromFile( assetPath.string() );

	AssetType assetId = serializer.Read<AssetType>( "type" );

	switch( assetId )
	{
		case AssetType::Texture:
		{
			auto asset = std::make_unique<TextureAsset>();
			asset->Load( systemManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::Mesh:
		{
			auto asset = std::make_unique<Mesh>();
			asset->Load( systemManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::Model:
		{
			auto asset = std::make_unique<Model>();
			asset->Load( systemManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::Material:
		{
			auto asset = std::make_unique<Material>();
			asset->Load( systemManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::MaterialTemplate:
		{
			auto asset = std::make_unique<MaterialTemplate>( *nodeLibrary );
			asset->Load( systemManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::Modifier:
		{
			auto asset = std::make_unique<Modifier>();
			asset->Load( systemManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::ModifierTemplate:
		{
			auto asset = std::make_unique<ModifierTemplate>();
			asset->Load( systemManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::Scene:
		{
			auto asset = std::make_unique<Scene>();
			asset->Load( systemManager, serializer );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
	}
}

void AssetManager::ImportMesh( const std::string& path, const std::string& name )
{
	auto meshData = importManager->LoadMesh( path );
	auto mesh = std::make_unique<Mesh>( meshData, name );
	for( const auto& meshDatum : meshData )
	{
		auto vbId = resourceManager->CreateVertexBuffer( meshDatum.vertices, name );
		auto ibId = resourceManager->CreateIndexBuffer( meshDatum.indices, name );

		auto subMesh = std::make_unique<SubMesh>( vbId, ibId, name );
		mesh->AddSubAsset( std::move( subMesh ) );
	}
	auto id = RegisterAsset( std::move( mesh ) );
	SaveAsset( id );
}

void AssetManager::ImportTexture( const std::string& path, const std::string& name )
{
	auto image = importManager->LoadTexture( path );
	auto texture = std::make_unique<TextureAsset>( name );
	auto texId = resourceManager->CreateTexture( std::move( image ), name );
	texture->AddResource( texId );
	auto id = RegisterAsset( std::move( texture ) );
	SaveAsset( id, "textures" );
}

// Register an asset, assign a unique ID, and check for duplicate pointers
AssetID AssetManager::RegisterAsset( std::unique_ptr<Asset> asset )
{
	if( !asset ) return {};

	std::lock_guard<std::mutex> lock( assetHeapMutex );

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

	std::lock_guard<std::mutex> lock( assetHeapMutex );

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
	std::lock_guard<std::mutex> lock( assetHeapMutex );
	assetHeap.erase( id );
}

const Asset* AssetManager::GetAsset( const AssetID& id ) const
{
	std::lock_guard<std::mutex> lock( assetHeapMutex );
	auto it = assetHeap.find( id );
	return (it != assetHeap.end()) ? it->second.get() : nullptr;
}

Asset* AssetManager::GetAsset( const AssetID& id )
{
	std::lock_guard<std::mutex> lock( assetHeapMutex );
	auto it = assetHeap.find( id );
	return (it != assetHeap.end()) ? it->second.get() : nullptr;
}

const std::vector<Asset*> AssetManager::GetAllAssets() const
{
	return GetAllAssets();
}

std::vector<Asset*> AssetManager::GetAllAssets()
{
	std::lock_guard<std::mutex> lock( assetHeapMutex );

	// 1. Build dependency graph
	std::unordered_map<AssetID, Asset*> idToAsset;
	std::unordered_map<AssetID, std::vector<AssetID>> dependencies;
	std::unordered_map<AssetID, int> inDegree;

	// Map AssetID to Asset* and initialize inDegree
	for( const auto& pair : assetHeap ) {
		idToAsset[pair.first] = pair.second.get();
		inDegree[pair.first] = 0;
	}

	// Build dependencies and in-degree
	for( const auto& pair : assetHeap ) {
		AssetID id = pair.first;
		auto* asset = pair.second.get();
		auto deps = asset->GetAllAssetIDs();
		dependencies[id] = deps;
		for( const auto& dep : deps ) {
			if( inDegree.count( dep ) ) // Only count dependencies that exist in the heap
				inDegree[dep]++;
		}
	}

	// 2. Kahn's algorithm for topological sort
	std::vector<Asset*> sorted;
	std::queue<AssetID> q;

	// Start with assets that have no dependencies
	for( const auto& [id, deg] : inDegree ) {
		if( deg == 0 )
			q.push( id );
	}

	while( !q.empty() ) {
		AssetID id = q.front();
		q.pop();
		sorted.push_back( idToAsset[id] );
		for( const auto& dep : dependencies[id] ) {
			if( inDegree.count( dep ) ) {
				inDegree[dep]--;
				if( inDegree[dep] == 0 )
					q.push( dep );
			}
		}
	}

	// If not all assets are sorted, there is a cycle or missing dependency
	if( sorted.size() != assetHeap.size() ) {
		// Optionally, handle cycles or missing dependencies here
		// For now, append remaining assets in any order
		for( const auto& pair : assetHeap ) {
			if( std::find( sorted.begin(), sorted.end(), pair.second.get() ) == sorted.end() ) {
				sorted.push_back( pair.second.get() );
			}
		}
	}

	return sorted;
}

AssetID AssetManager::GenerateUniqueAssetId()
{
	return "Asset_" + std::to_string( assetIdCounter++ );
}
