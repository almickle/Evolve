#include <exception>
#include <stdexcept>
#include <string>
#include "Asset.h"
#include "JsonSerializer.h"
#include "Modifier.h"
#include "SystemManager.h"
#include "Types.h"

void Modifier::Load( SystemManager* systemManager, JsonSerializer& serializer )
{
	Deserialize( serializer );
}

std::string Modifier::Serialize( JsonSerializer& serializer ) const
{
	serializer.StartDocument();
	SerializeBaseAsset( serializer );
	serializer.Write( "modifierTemplate", modifierTemplate );
	serializer.EndDocument();
	return serializer.GetString();
}

void Modifier::Deserialize( JsonSerializer& serializer )
{
	try
	{
		DeserializeBaseAsset( serializer );
		modifierTemplate = serializer.Read<AssetID>( "modifierTemplate" );
	}
	catch( const std::exception& )
	{
		throw std::runtime_error( "Failed to deserialize Modifier" );
	}
}