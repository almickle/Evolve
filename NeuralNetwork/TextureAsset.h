#include <string>
#include "Asset.h"
#include "Types.h"

class GpuResourceManager;

class TextureAsset : public Asset {
public:
	TextureAsset( const std::string& name = "TextureAsset" )
		: Asset( AssetType::Texture, name )
	{
	}
	~TextureAsset() = default;
public:
	void Load( const std::string& filePath, GpuResourceManager& resourceManager ) override;
};