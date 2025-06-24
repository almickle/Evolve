#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "NodeTypes.h"
#include "ShaderNode.h"
#include "Types.h"

class VectorMakeNode :
	public ShaderNode
{
public:
	VectorMakeNode( const std::string& name = "VectorMakeNode" )
		: ShaderNode( NodeTypes::VectorMake, 3, 1, 0, name )
	{
		AddInput( xInputSlot, NodeSlot( "x", 0.0f ) );
		AddInput( yInputSlot, NodeSlot( "y", 0.0f ) );
		AddInput( zInputSlot, NodeSlot( "z", 0.0f ) );
		AddOutput( vectorOutputSlot, NodeSlot( "vec", DirectX::XMFLOAT4{ 0, 0, 0, 0 } ) );
	}
	~VectorMakeNode() {}
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody =
			std::format( " output.{} = float4(input.{}, input.{}, input.{}, 0.0f);\n", GetOutput( vectorOutputSlot ).name, GetInput( xInputSlot ).name, GetInput( yInputSlot ).name, GetInput( zInputSlot ).name );

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	uint xInputSlot = 0;
	uint yInputSlot = 1;
	uint zInputSlot = 2;
private:
	uint vectorOutputSlot = 0;
};

