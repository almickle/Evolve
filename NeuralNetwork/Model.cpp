#include <exception>
#include <stdexcept>
#include <string>
#include "JsonSerializer.h"
#include "Model.h"
#include "SystemManager.h"
#include "Types.h"

std::string Model::Serialize( JsonSerializer& serializer ) const
{
	serializer.StartDocument();
	serializer.BeginObject();

	// Meta data and asset ids
	SerializeBaseAsset( serializer );

	// Mesh reference (by asset ID)
	serializer.Write( "meshId", meshId );

	// Material slots (array of asset IDs)
	serializer.WriteArray( "materialSlots", materialSlots );
	serializer.WriteArray( "modifierSlots", modifierSlots );

	serializer.EndObject();
	serializer.EndDocument();

	return serializer.GetString();
}

void Model::Load( SystemManager* systemManager, JsonSerializer& serializer )
{
	Deserialize( serializer );
}

void Model::Deserialize( JsonSerializer& serializer )
{
	try
	{
		DeserializeBaseAsset( serializer );

		// Mesh reference
		meshId = serializer.Read<AssetID>( "meshId" );

		// Material slots
		materialSlots = serializer.ReadArray<AssetID>( "materialSlots" );
		modifierSlots = serializer.ReadArray<AssetID>( "modifierSlots" );
	}
	catch( const std::exception& )
	{
		throw std::runtime_error( "Failed to deserialize Model" );
	}

}