#pragma once
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
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
	//auto entries = std::filesystem::directory_iterator( assetDirectory );

	namespace fs = std::filesystem;
	auto entries = fs::recursive_directory_iterator( assetDirectory, fs::directory_options::skip_permission_denied );
	for( const auto& entry : entries )
	{
		if( entry.is_regular_file() && entry.path().extension() == assetFileExtension )
		{
			std::string path = std::filesystem::absolute( entry.path() ).string();
			Task task;
			task.path = path;
			task.taskFunc = [this, systemManager]( const std::string& filePath ) {
				JsonSerializer localSerializer;
				this->LoadAsset( filePath, systemManager );
				};
			// Optionally, set onComplete if you want a callback after loading
			task.onComplete = nullptr;
			taskManager->Enqueue( std::move( task ) );
		}
	}
	auto fence = taskManager->InsertFence();
	fence.wait();

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

void AssetManager::LoadAsset( const std::string& path, SystemManager* systemManager )
{
	auto* serializer = systemManager->GetSerializer();

	std::filesystem::path assetPath = assetDirectory / path;
	serializer->LoadFromFile( assetPath.string() );

	AssetType assetId = serializer->Read<AssetType>( "type" );

	switch( assetId )
	{
		case AssetType::Texture:
		{
			auto asset = std::make_unique<TextureAsset>();
			asset->Load( systemManager );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::Mesh:
		{
			auto asset = std::make_unique<Mesh>();
			asset->Load( systemManager );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::Model:
		{
			auto asset = std::make_unique<Model>();
			asset->Load( systemManager );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::Material:
		{
			auto asset = std::make_unique<Material>();
			asset->Load( systemManager );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::MaterialTemplate:
		{
			auto asset = std::make_unique<MaterialTemplate>( *nodeLibrary );
			asset->Load( systemManager );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::Modifier:
		{
			auto asset = std::make_unique<Modifier>();
			asset->Load( systemManager );
			auto id = asset->GetAssetID();
			RegisterAsset( id, std::move( asset ) );
		}
		break;
		case AssetType::ModifierTemplate:
		{
			auto asset = std::make_unique<ModifierTemplate>();
			asset->Load( systemManager );
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
	std::lock_guard<std::mutex> lock( assetHeapMutex );
	std::vector<Asset*> all;
	for( const auto& pair : assetHeap ) {
		all.push_back( pair.second.get() );
	}
	return all;
}

std::vector<Asset*> AssetManager::GetAllAssets()
{
	std::lock_guard<std::mutex> lock( assetHeapMutex );
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
