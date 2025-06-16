#include <string>
#include "Asset.h"
#include "JsonSerializer.h"
#include "Modifier.h"
#include "SystemManager.h"
#include "Types.h"

void Modifier::Load( SystemManager* systemManager )
{
	Deserialize( *systemManager->GetSerializer() );
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
	DeserializeBaseAsset( serializer );
	modifierTemplate = serializer.Read<AssetID>( "modifierTemplate" );
}