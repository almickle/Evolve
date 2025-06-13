#include <string>
#include "GpuResourceManager.h"
#include "JsonSerializer.h"
#include "Model.h"
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

	serializer.EndObject();
	serializer.EndDocument();

	return serializer.GetString();
}

void Model::Load( GpuResourceManager& resourceManager, JsonSerializer& serializer )
{
	Deserialize( serializer );
}

void Model::Deserialize( JsonSerializer& serializer )
{
	DeserializeBaseAsset( serializer );

	// Mesh reference
	meshId = serializer.Read<AssetID>( "meshId" );

	// Material slots
	materialSlots = serializer.ReadArray<AssetID>( "materialSlots" );
}