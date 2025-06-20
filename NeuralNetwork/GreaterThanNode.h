#pragma once
#include <format>
#include <string>
#include "NodeTypes.h"
#include "ShaderNode.h"
#include "Types.h"

class GreaterThanNode : public ShaderNode {
public:
	GreaterThanNode( const std::string& name = "GreaterThanNode" )
		: ShaderNode( 2, 1, 0, name )
	{
		AddInput( valueAInputSlot, NodeSlot{ "value_a", float( 0 ) } );
		AddInput( valueBInputSlot, NodeSlot{ "value_b", float( 0 ) } );
		AddOutput( resultOutputSlot, NodeSlot{ "result", false } );
	}
	~GreaterThanNode() = default;
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format(
			"output.{} = input.{} > input.{};\n",
			GetResultOutput().name,
			GetValueAInput().name,
			GetValueBInput().name );

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	NodeSlot GetValueAInput() const { return inputs[valueAInputSlot]; }
	NodeSlot GetValueBInput() const { return inputs[valueBInputSlot]; }
	NodeSlot GetResultOutput() const { return outputs[resultOutputSlot]; }
private:
	uint valueAInputSlot = 0;
	uint valueBInputSlot = 1;
private:
	uint resultOutputSlot = 0;
};