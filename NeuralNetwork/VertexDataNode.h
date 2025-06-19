#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "ShaderNode.h"
#include "NodeTypes.h"
#include "Types.h"

class VertexDataNode :
	public ShaderNode
{
public:
	VertexDataNode( const std::string& name = "VertexDataNode" )
		: ShaderNode( 0, 5, 0, name )
	{
		AddOutput( pixelPosOutputSlot, NodeSlot{ "pixelPos", DirectX::XMFLOAT4{ 0, 0, 0, 0 } } );
		AddOutput( worldPosOutputSlot, NodeSlot{ "worldPos", DirectX::XMFLOAT4{ 0, 0, 0,0 } } );
		AddOutput( normalOutputSlot, NodeSlot{ "normal", DirectX::XMFLOAT4{ 0, 0, 0, 0 } } );
		AddOutput( uvOutputSlot, NodeSlot{ "uv",  DirectX::XMFLOAT4{ 0, 0, 0, 0 } } );
		AddOutput( tangentOutputSlot, NodeSlot{ "tangent", DirectX::XMFLOAT4{ 0, 0, 0, 1 } } );
	}
	~VertexDataNode() {}
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody =
			"output.pixelPos = vertexData.pixelPos;\n"
			"output.worldPos = float4(vertexData.worldPos, 0.0f);\n"
			"output.normal = float4(vertexData.normal, 0.0f);\n"
			"output.uv = float4(vertexData.uv, 0.0f, 0.0f);\n"
			"output.tangent = vertexData.tangent;\n";

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	uint pixelPosOutputSlot = 0;
	uint worldPosOutputSlot = 1;
	uint normalOutputSlot = 2;
	uint uvOutputSlot = 3;
	uint tangentOutputSlot = 4;
};

