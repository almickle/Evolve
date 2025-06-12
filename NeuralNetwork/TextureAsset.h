#include <string>
#include "Asset.h"
#include "ImportManager.h"
#include "JsonSerializer.h"
#include "Types.h"

class GpuResourceManager;

class TextureAsset : public Asset {
public:
	TextureAsset( ImportManager& importManager, const std::string& name = "TextureAsset" )
		: Asset( AssetType::Texture, name ),
		importManager( importManager )
	{
	}
	~TextureAsset() = default;
public:
	void Load( GpuResourceManager& resourceManager, JsonSerializer& serializer ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
private:
	std::string texturePath;
	ImportManager& importManager;
};