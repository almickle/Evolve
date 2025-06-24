#pragma once
#include <format>
#include <string>
#include "NodeTypes.h"
#include "ShaderNode.h"
#include "Types.h"

class SignNode : public ShaderNode {
public:
	SignNode( const std::string& name = "SignNode" )
		: ShaderNode( NodeTypes::Sign, 1, 1, 0, name )
	{
		AddInput( valueInputSlot, NodeSlot( "value", float( 0 ) ) );
		AddOutput( resultOutputSlot, NodeSlot( "result", float( 0 ) ) );
	}
	~SignNode() = default;
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format( "output.{} = (input.{} > 0) - (input.{} < 0);\n",
												GetResultOutput().name,
												GetValueInput().name,
												GetValueInput().name );

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	NodeSlot GetValueInput() const { return inputs[valueInputSlot]; }
	NodeSlot GetResultOutput() const { return outputs[resultOutputSlot]; }
private:
	uint valueInputSlot = 0;
private:
	uint resultOutputSlot = 0;
};