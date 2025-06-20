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
		: ShaderNode( 2, 7, 0, name )
	{
		AddInput( resInputSlot, NodeSlot{ "resolution", DirectX::XMFLOAT4{} } );
		AddInput( sizeInputSlot, NodeSlot{ "size", 0.0f } );
		AddOutput( pixelPosOutputSlot, NodeSlot{ "pixelPos", DirectX::XMFLOAT4{} } );
		AddOutput( worldPosOutputSlot, NodeSlot{ "worldPos", DirectX::XMFLOAT4{} } );
		AddOutput( normalOutputSlot, NodeSlot{ "normal", DirectX::XMFLOAT4{} } );
		AddOutput( uvOutputSlot, NodeSlot{ "uv",  DirectX::XMFLOAT4{} } );
		AddOutput( tangentOutputSlot, NodeSlot{ "tangent", DirectX::XMFLOAT4{ 0, 0, 0, 1 } } );
		AddOutput( tri0OutputSlot, NodeSlot{ "triangle_1", DirectX::XMUINT3{} } );
		AddOutput( tri1OutputSlot, NodeSlot{ "triangle_2", DirectX::XMUINT3{} } );
	}
	~MeshPlanePrimitiveNode() = default;
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format(
			"uint2 gvid = threadIds.dtid.xy;\n"
			"uint2 lvid = threadIds.tid.xy;\n"
			"float2 uv = (float2) gvid / float2(input.{} - 1);\n"
			"float2 gridPos = (uv - 0.5) * input.{};\n"
			"float3 worldPos = float3(gridPos, 0);\n"
			"// Assemble triangles\n"
			"uint2 p0 = uint2(lvid.x, lvid.y)\n"
			"uint2 p1 = uint2(lvid.x + 1, lvid.y);\n"
			"uint2 p2 = uint2(lvid.x + 1, lvid.y + 1);\n"
			"uint2 p3 = uint2(lvid.x, lvid.y + 1);\n"
			"uint i0 = p0.x + p0.y * THREADS_X;\n"
			"uint i1 = p1.x + p1.y * THREADS_X;\n"
			"uint i2 = p2.x + p2.y * THREADS_X;\n"
			"uint i3 = p3.x + p3.y * THREADS_X;\n"
			"output.{} = mul(viewProj, float4(worldPos, 1.0));\n"
			"output.{} = worldPos;\n"
			"output.{} = float3(0, 0, 1);\n"
			"output.{} = float4(1, 0, 0, 1);\n"
			"output.{} = uv;\n"
			"output.{} = uint3(i0, i1, i3);\n"
			"output.{} = uint3(i0, i2, i3);\n",
			GetResolutionInput().name,
			GetSizeInput().name,
			GetPixelPosOutput().name,
			GetWorldPosOutput().name,
			GetNormalOutput().name,
			GetTangentOutput().name,
			GetUVOutput().name,
			GetTri0Output().name,
			GetTri1Output().name
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
	NodeSlot GetTri0Output() const { return outputs[tri0OutputSlot]; }
	NodeSlot GetTri1Output() const { return outputs[tri1OutputSlot]; }
private:
	uint resInputSlot = 0;
	uint sizeInputSlot = 1;
private:
	uint pixelPosOutputSlot = 0;
	uint worldPosOutputSlot = 1;
	uint normalOutputSlot = 2;
	uint uvOutputSlot = 3;
	uint tangentOutputSlot = 4;
	uint tri0OutputSlot = 5;
	uint tri1OutputSlot = 6;
};