#include <string>
#include "GpuResourceManager.h"
#include "JsonSerializer.h"
#include "TextureAsset.h"

std::string TextureAsset::Serialize( JsonSerializer& serializer ) const
{
	serializer.StartDocument();
	serializer.BeginObject();

	// Meta data and asset ids
	SerializeBaseAsset( serializer );

	serializer.Write( "texturePath", texturePath );

	serializer.EndObject();
	serializer.EndDocument();

	return serializer.GetString();
}

void TextureAsset::Load( GpuResourceManager& resourceManager, JsonSerializer& serializer )
{
	Deserialize( serializer );
	auto data = importManager.LoadTexture( texturePath );
	auto resourceId = resourceManager.CreateTexture( data.subresources, data.texDesc, name );
	AddResource( resourceId );
}

void TextureAsset::Deserialize( JsonSerializer& serializer )
{
	DeserializeBaseAsset( serializer );

	texturePath = serializer.Read<std::string>( "texturePath" );
}