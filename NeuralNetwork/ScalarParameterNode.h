#pragma once
#include <format>
#include <string>
#include "NodeTypes.h"
#include "ShaderNode.h"
#include "Types.h"

class ScalarParameterNode :
	public ShaderNode
{
public:
	ScalarParameterNode( const std::string& name = "ScalarParameterNode" )
		: ShaderNode( NodeTypes::ScalarParameter, 0, 1, 1, name )
	{
		AddOutput( valueOutputSlot, NodeSlot( "value", 0.0f ) );
		AddParameter( scalarParameterSlot, NodeParameter( NodeParameterType::Scalar, "scalar" ) );
	}
	~ScalarParameterNode() {}
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format( " output.{} = parameters.{};\n", GetOutput( valueOutputSlot ).name, GetParameter( scalarParameterSlot ).name );
		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	uint valueOutputSlot = 0;
private:
	uint scalarParameterSlot = 0;
};

