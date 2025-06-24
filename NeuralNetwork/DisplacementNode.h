#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "NodeTypes.h"
#include "ShaderNode.h"
#include "Types.h"

class DisplacementNode : public ShaderNode {
public:
	DisplacementNode( const std::string& name = "DisplacementNode" )
		: ShaderNode( NodeTypes::Displacement, 4, 1, 0, name )
	{
		AddInput( heightInputSlot, NodeSlot( "height", 0.0f ) );
		AddInput( scaleInputSlot, NodeSlot( "scale", 0.0f ) );
		AddInput( positionInputSlot, NodeSlot( "position", DirectX::XMFLOAT4{} ) );
		AddInput( directionInputSlot, NodeSlot( "direction", DirectX::XMFLOAT4{} ) );
		AddOutput( positionOutputSlot, NodeSlot( "position", DirectX::XMFLOAT4{} ) );
	}
	~DisplacementNode() = default;
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format( "output.{} = input.{} + (normalize(input.{}) * input.{} * input.{});\n",
												GetPositionOutputSlot().name,
												GetPositionInputSlot().name,
												GetDirectionInputSlot().name,
												GetHeightInputSlot().name,
												GetScaleInputSlot().name );

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	NodeSlot GetHeightInputSlot() const { return inputs[heightInputSlot]; }
	NodeSlot GetScaleInputSlot() const { return outputs[scaleInputSlot]; }
	NodeSlot GetPositionInputSlot() const { return inputs[positionInputSlot]; }
	NodeSlot GetDirectionInputSlot() const { return inputs[directionInputSlot]; }
	NodeSlot GetPositionOutputSlot() const { return outputs[positionOutputSlot]; }
private:
	uint heightInputSlot = 0;
	uint scaleInputSlot = 1;
	uint positionInputSlot = 2;
	uint directionInputSlot = 3;
private:
	uint positionOutputSlot = 0;
};