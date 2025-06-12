#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "MaterialNode.h"
#include "NodeTypes.h"
#include "Types.h"

class TextureSamplerNode : public MaterialNode {
public:
	TextureSamplerNode( const std::string& name = "TextureSamplerNode" )
		: MaterialNode( 1, 1, 1, name )
	{
		AddInput( uvInputSlotIndex, NodeSlot{ "uv", DirectX::XMFLOAT2{ 0.0f, 0.0f } } );
		AddOutput( colorOutputSlotIndex, NodeSlot{ "color", DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f } } );
		AddParameter( textureParameter, NodeParameter( NodeParameterTypes::Texture, "textureIndex" ) );
	}
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format( "output.{} = textures[parameters.{}].Sample(samp, input.{});\n", GetColorOutputSlot().name, GetTextureParameter().name, GetUVSlot().name );

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
	NodeParameter GetTextureParameter() const { return parameters[textureParameter]; }
	NodeSlot GetUVSlot() const { return inputs[uvInputSlotIndex]; }
	NodeSlot GetColorOutputSlot() const { return outputs[colorOutputSlotIndex]; }
private:
	uint uvInputSlotIndex = 0;
private:
	uint colorOutputSlotIndex = 0;
private:
	uint textureParameter = 0;
};