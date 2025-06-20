#pragma once
#include <format>
#include <string>
#include "NodeTypes.h"
#include "ShaderNode.h"
#include "Types.h"

class AndNode : public ShaderNode {
public:
	AndNode( const std::string& name = "AndNode" )
		: ShaderNode( 2, 1, 0, name )
	{
		AddInput( conditionAInputSlot, NodeSlot{ "condition_a", false } );
		AddInput( conditionBInputSlot, NodeSlot{ "condition_b", false } );
		AddOutput( resultOutputSlot, NodeSlot{ "result", false } );
	}
	~AndNode() = default;
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format(
			"output.{} = input.{} && input.{};\n",
			GetResultOutput().name,
			GetConditionAInput().name,
			GetConditionBInput().name );

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	NodeSlot GetConditionAInput() const { return inputs[conditionAInputSlot]; }
	NodeSlot GetConditionBInput() const { return inputs[conditionBInputSlot]; }
	NodeSlot GetResultOutput() const { return outputs[resultOutputSlot]; }
private:
	uint conditionAInputSlot = 0;
	uint conditionBInputSlot = 1;
private:
	uint resultOutputSlot = 0;
};