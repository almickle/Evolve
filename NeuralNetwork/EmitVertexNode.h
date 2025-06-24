#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "NodeTypes.h"
#include "ShaderNode.h"
#include "Types.h"

class EmitVertexNode : public ShaderNode {
public:
	EmitVertexNode( const std::string& name = "EmitVertexNode" )
		: ShaderNode( NodeTypes::EmitVertex, 6, 0, 0, name )
	{
		AddInput( pixelPosInputSlot, NodeSlot( "pixelPos", DirectX::XMFLOAT4{} ) );
		AddInput( worldPosInputSlot, NodeSlot( "worldPos", DirectX::XMFLOAT4{} ) );
		AddInput( normalInputSlot, NodeSlot( "normal", DirectX::XMFLOAT4{} ) );
		AddInput( uvInputSlot, NodeSlot( "uv", DirectX::XMFLOAT4{} ) );
		AddInput( tangentInputSlot, NodeSlot( "tangent", DirectX::XMFLOAT4{ 0, 0, 0, 1 } ) );
		AddInput( conditionInputSlot, NodeSlot( "condition", false ) );
	}
	~EmitVertexNode() = default;
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format( "if(!input.{}) return;\n"
												"uint2 lvid = threadIds.tid.xy;\n"
												"uint index = lvid.x + lvid.y * THREADS_X;\n"
												"VSOutput vertex;\n"
												"vertex.pixelPos = input.{};\n"
												"vertex.worldPos = input.{};\n"
												"vertex.normal = input.{};\n"
												"vertex.uv = input.{};\n"
												"vertex.tangent = input.{};\n"
												"verts[index] = vertex;\n",
												GetConditionInput().name,
												GetPixelPositionInput().name,
												GetWorldPositionInput().name,
												GetNormalInput().name,
												GetUVInput().name,
												GetTangentInput().name );

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	NodeSlot GetPixelPositionInput() const { return inputs[pixelPosInputSlot]; }
	NodeSlot GetWorldPositionInput() const { return inputs[worldPosInputSlot]; }
	NodeSlot GetNormalInput() const { return inputs[normalInputSlot]; }
	NodeSlot GetUVInput() const { return inputs[uvInputSlot]; }
	NodeSlot GetTangentInput() const { return inputs[tangentInputSlot]; }
	NodeSlot GetConditionInput() const { return inputs[conditionInputSlot]; }
private:
	uint pixelPosInputSlot = 0;
	uint worldPosInputSlot = 1;
	uint normalInputSlot = 2;
	uint uvInputSlot = 3;
	uint tangentInputSlot = 4;
	uint conditionInputSlot = 5;
};