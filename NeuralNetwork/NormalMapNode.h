#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "MaterialNode.h"
#include "NodeTypes.h"
#include "Types.h"

class NormalMapNode :
	public MaterialNode
{
public:
	NormalMapNode( const std::string& name = "NormalMapNode" )
		: MaterialNode( 1, 1, 0, name )
	{
		AddInput( normalInputSlot, NodeSlot{ "color",  DirectX::XMFLOAT4{ 0, 0, 0, 0} } );
		AddOutput( normalOutputSlot, NodeSlot{ "normal",  DirectX::XMFLOAT4{ 0, 0, 0, 0 } } );
	}
	~NormalMapNode() {}
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody =
			"float3 T = normalize( vertexData.tangent.xyz );\n"
			"float3 N = normalize( vertexData.normal.xyz );\n"
			"float3 B = normalize( cross( N, T ) * vertexData.tangent.w );\n"
			"float3x3 TBN = float3x3( T, B, N );\n" +
			std::format( "output.{} = float4(mul(TBN, input.{}.rgb), 0.0f);\n", GetOutput( normalOutputSlot ).name, GetInput( normalInputSlot ).name );

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	uint normalInputSlot = 0;
private:
	uint normalOutputSlot = 0;
};

