#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
#include "GpuResourceManager.h"
#include "ImportManager.h"
#include "JsonSerializer.h"
#include "SystemManager.h"
#include "Texture.h"
#include "TextureAsset.h"
#include "Types.h"

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

void TextureAsset::Load( SystemManager* systemManager, JsonSerializer& serializer )
{
	auto* importManager = systemManager->GetImportManager();
	auto* resourceManager = systemManager->GetResourceManager();

	Deserialize( serializer );
	auto data = importManager->LoadTexture( texturePath );
	textureId = resourceManager->CreateTexture( std::move( data ), name );
	AddResource( textureId );
}

void TextureAsset::Deserialize( JsonSerializer& serializer )
{
	try
	{
		DeserializeBaseAsset( serializer );
		texturePath = serializer.Read<std::string>( "texturePath" );
	}
	catch( const std::exception& )
	{
		throw std::runtime_error( "Failed to deserialize TextureAsset" );
	}
}

uint TextureAsset::GetSrvHeapIndex( GpuResourceManager& resourceManager ) const
{
	auto* tex = static_cast<Texture*>(resourceManager.GetResource( textureId ));
	return tex->GetSrvHeapIndex();
}