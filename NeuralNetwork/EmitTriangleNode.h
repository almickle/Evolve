#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "NodeTypes.h"
#include "ShaderNode.h"
#include "Types.h"

class EmitTriangleNode : public ShaderNode {
public:
	EmitTriangleNode( const std::string& name = "EmitTriangleNode" )
		: ShaderNode( NodeTypes::EmitTriangle, 3, 0, 0, name )
	{
		AddInput( conditionInputSlot, NodeSlot( "condition", false ) );
		AddInput( indexInputSlot, NodeSlot( "index", uint( 0 ) ) );
		AddInput( triangleInputSlot, NodeSlot( "triangle", DirectX::XMUINT3{} ) );
	}
	~EmitTriangleNode() = default;
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format(
			"if(!input.{}) return;\n"
			"tris[input.{}] = input.{};\n",
			GetConditionInput().name,
			GetIndexInput().name,
			GeTriangleInput().name );

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	NodeSlot GetConditionInput() const { return inputs[conditionInputSlot]; }
	NodeSlot GetIndexInput() const { return inputs[indexInputSlot]; }
	NodeSlot GeTriangleInput() const { return inputs[triangleInputSlot]; }
private:
	uint conditionInputSlot = 0;
	uint indexInputSlot = 1;
	uint triangleInputSlot = 2;
};