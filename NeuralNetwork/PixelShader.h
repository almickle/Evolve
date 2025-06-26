#pragma once
#include <string>
#include "NodeLibrary.h"
#include "ShaderGraph.h"
#include "Types.h"

class PixelShader :
	public ShaderGraph
{
public:
	PixelShader( NodeLibrary& nodeLibrary, const std::string& name = "Pixel Shader" )
		: ShaderGraph( nodeLibrary, AssetType::PixelShader, ShaderType::Pixel, name, "float4 main(VSOutput vertexData) : SV_TARGET" )
	{
		AddInclude( "Common.hlsli" );
	}
	~PixelShader() = default;
};