#pragma once
#include <string>
#include <vector>
#include "ShaderBindings.h"
#include "ShaderInstance.h"
#include "Types.h"

class Material :
	public ShaderInstance {
public:
	Material( const std::string& name = "Material" )
		: ShaderInstance( AssetType::Material, name )
	{
	}
	Material( const AssetID& pixelShader,
			  const std::vector<TextureBinding>& textureBindings,
			  const std::vector<VectorBinding>& vectorBindings,
			  const std::vector<ScalarBinding>& scalarBindings,
			  const std::string& name = "Material" )
		: ShaderInstance(
			pixelShader,
			AssetType::Material,
			textureBindings,
			vectorBindings,
			scalarBindings,
			name )
	{
	}
	~Material() = default;
};