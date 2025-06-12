#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "Asset.h"
#include "JsonSerializer.h"
#include "SubAsset.h"
#include "Types.h"

std::vector<ResourceID> Asset::GetAllResourceIDs() const
{
	std::vector<ResourceID> allIDs = resourceIDs;
	for( const auto& sub : subassets ) {
		const auto& subIDs = sub->GetResourceIDs();
		allIDs.insert( allIDs.end(), subIDs.begin(), subIDs.end() );
	}
	return allIDs;
}

void Asset::Save( const std::filesystem::path& dirPath, const std::string& content ) const
{
	// Use std::filesystem for path manipulation (C++17 and later)
	namespace fs = std::filesystem;
	fs::path filename = name + ".json";
	fs::path fullPath = dirPath / filename;

	std::ofstream outFile( fullPath, std::ios::out | std::ios::trunc );
	if( !outFile.is_open() ) {
		// Optionally, handle error (log, throw, etc.)
		return;
	}
	outFile << content;
	outFile.close();
}

void Asset::SerializeBaseAsset( JsonSerializer& serializer ) const
{
	// Meta data
	serializer.Write( "id", id );
	serializer.Write( "name", name );
	serializer.Write( "type", static_cast<int>(assetType) );

	// Asset IDs
	serializer.WriteArray( "assetIds", assetIds );
}

void Asset::DeserializeBaseAsset( JsonSerializer& serializer )
{
	id = serializer.Read<std::string>( "id" );
	name = serializer.Read<std::string>( "name" );
	assetType = static_cast<AssetType>(serializer.Read<int>( "type" ));
	assetIds = serializer.ReadArray<AssetID>( "assetIds" );
}