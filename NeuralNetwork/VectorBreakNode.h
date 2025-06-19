#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "ShaderNode.h"
#include "NodeTypes.h"
#include "Types.h"

class VectorBreakNode :
	public ShaderNode
{
public:
	VectorBreakNode( const std::string& name = "VectorBreakNode" )
		: ShaderNode( 1, 3, 0, name )
	{
		AddInput( vectorInputSlot, NodeSlot{ "vec",  DirectX::XMFLOAT4{ 0, 0, 0, 0 } } );
		AddOutput( xOutputSlot, NodeSlot{ "x", 0.0f } );
		AddOutput( yOutputSlot, NodeSlot{ "y", 0.0f } );
		AddOutput( zOutputSlot, NodeSlot{ "z", 0.0f } );
	}
	~VectorBreakNode() {}
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody =
			std::format( " output.{} = input.{}.x;\n", GetOutput( xOutputSlot ).name, GetInput( vectorInputSlot ).name ) +
			std::format( " output.{} = input.{}.y;\n", GetOutput( yOutputSlot ).name, GetInput( vectorInputSlot ).name ) +
			std::format( " output.{} = input.{}.z;\n", GetOutput( zOutputSlot ).name, GetInput( vectorInputSlot ).name );

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	uint vectorInputSlot = 0;
private:
	uint xOutputSlot = 0;
	uint yOutputSlot = 1;
	uint zOutputSlot = 2;
};

