#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "NodeTypes.h"
#include "ShaderNode.h"
#include "Types.h"

class MeshPlanePrimitiveNode : public ShaderNode {
public:
	MeshPlanePrimitiveNode( const std::string& name = "MeshPlanePrimitiveNode" )
		: ShaderNode( 2, 5, 0, name )
	{
		AddInput( resInputSlot, NodeSlot{ "resolution", DirectX::XMFLOAT4{} } );
		AddInput( sizeInputSlot, NodeSlot{ "size", 0.0f } );
		AddOutput( pixelPosOutputSlot, NodeSlot{ "pixelPos", DirectX::XMFLOAT4{} } );
		AddOutput( worldPosOutputSlot, NodeSlot{ "worldPos", DirectX::XMFLOAT4{} } );
		AddOutput( normalOutputSlot, NodeSlot{ "normal", DirectX::XMFLOAT4{} } );
		AddOutput( uvOutputSlot, NodeSlot{ "uv",  DirectX::XMFLOAT4{} } );
		AddOutput( tangentOutputSlot, NodeSlot{ "tangent", DirectX::XMFLOAT4{ 0, 0, 0, 1 } } );
	}
	~MeshPlanePrimitiveNode() = default;
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format(
			"uint2 vid = threadIds.dtid.xy;\n"
			"float2 uv = (float2) vid / float2(input.{} - 1);\n"
			"float2 gridPos = (uv - 0.5) * input.{};\n"
			"float3 worldPos = float3(gridPos, 0);\n"
			"output.{} = mul(viewProj, float4(worldPos, 1.0));\n"
			"output.{} = worldPos;\n"
			"output.{} = float3(0, 0, 1);\n"
			"output.{} = float4(1, 0, 0, 1);\n"
			"output.{} = uv;\n",
			GetResolutionInput().name,
			GetSizeInput().name,
			GetPixelPosOutput().name,
			GetWorldPosOutput().name,
			GetNormalOutput().name,
			GetTangentOutput().name,
			GetUVOutput().name
		);

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	NodeSlot GetResolutionInput() const { return inputs[resInputSlot]; }
	NodeSlot GetSizeInput() const { return inputs[sizeInputSlot]; }
	NodeSlot GetPixelPosOutput() const { return outputs[pixelPosOutputSlot]; }
	NodeSlot GetWorldPosOutput() const { return outputs[worldPosOutputSlot]; }
	NodeSlot GetNormalOutput() const { return outputs[normalOutputSlot]; }
	NodeSlot GetUVOutput() const { return outputs[uvOutputSlot]; }
	NodeSlot GetTangentOutput() const { return outputs[tangentOutputSlot]; }
private:
	uint resInputSlot = 0;
	uint sizeInputSlot = 1;
private:
	uint pixelPosOutputSlot = 0;
	uint worldPosOutputSlot = 1;
	uint normalOutputSlot = 2;
	uint uvOutputSlot = 3;
	uint tangentOutputSlot = 4;
};