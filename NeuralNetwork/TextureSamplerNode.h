#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "NodeTypes.h"
#include "ShaderNode.h"
#include "Types.h"

class TextureSamplerNode : public ShaderNode {
public:
	TextureSamplerNode( const std::string& name = "TextureSamplerNode" )
		: ShaderNode( NodeTypes::TextureSampler, 1, 1, 1, name )
	{
		AddInput( uvInputSlotIndex, NodeSlot( "uv", DirectX::XMFLOAT4{ 0, 0, 0, 0 } ) );
		AddOutput( colorOutputSlotIndex, NodeSlot( "color", DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f } ) );
		AddParameter( textureParameter, NodeParameter( NodeParameterType::Texture, "textureIndex" ) );
	}
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format( "output.{} = textures[parameters.{}].Sample(samp, input.{}.xy);\n", GetColorOutputSlot().name, GetTextureParameter().name, GetUVSlot().name );

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