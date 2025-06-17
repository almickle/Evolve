#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "MaterialNode.h"
#include "NodeTypes.h"
#include "Types.h"

class VectorScaleNode :
	public MaterialNode
{
public:
	VectorScaleNode( const std::string& name = "VectorScaleNode" )
		: MaterialNode( 2, 1, 0, name )
	{
		AddInput( vectorInputSlot, NodeSlot{ "vec",  DirectX::XMFLOAT4{ 0, 0, 0, 0 } } );
		AddInput( scaleInputSlot, NodeSlot{ "scale", 1.0f } );
		AddOutput( vectorOutputSlot, NodeSlot{ "vec",  DirectX::XMFLOAT4{ 0, 0, 0, 0 } } );
	}
	~VectorScaleNode() {}
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format( " output.{} = float4(mul(input.{}.xyz, input.{}), 0.0f);\n", GetOutput( vectorOutputSlot ).name, GetInput( vectorInputSlot ).name, GetInput( scaleInputSlot ).name );
		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	uint vectorInputSlot = 0;
	uint scaleInputSlot = 1;
private:
	uint vectorOutputSlot = 0;
};

