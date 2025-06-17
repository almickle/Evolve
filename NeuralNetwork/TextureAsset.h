#include <string>
#include "Asset.h"
#include "GpuResourceManager.h"
#include "JsonSerializer.h"
#include "Types.h"

class SystemManager;

class TextureAsset : public Asset {
public:
	TextureAsset( const std::string& name = "TextureAsset" )
		: Asset( AssetType::Texture, name )
	{
	}
	~TextureAsset() = default;
public:
	void Load( SystemManager* systemManager, JsonSerializer& serializer ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
public:
	uint GetSrvHeapIndex( GpuResourceManager& resourceManager ) const;
private:
	std::string texturePath;
private:
	ResourceID textureId;
};