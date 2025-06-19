#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "ShaderNode.h"
#include "NodeTypes.h"
#include "Types.h"

class NormalMapNode :
	public ShaderNode
{
public:
	NormalMapNode( const std::string& name = "NormalMapNode" )
		: ShaderNode( 1, 1, 0, name )
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

		std::string functionBody = std::format( R"(
			// Build TBN basis (world space)
			float3 T = normalize(vertexData.tangent.xyz);
			float3 N = normalize(vertexData.normal.xyz);
			float3 B = normalize(cross(N, T) * vertexData.tangent.w);
			float3x3 TBN = float3x3(T, B, N);

			// Sampled normal is in [0,1] → remap to [-1,1]
			float3 nTS = input.{}.rgb * 2.0f - 1.0f;

			// Row-vector convention: rowVec * matrix
			float3 nWS = normalize(mul(nTS, TBN));
			output.{} = float4(nWS, 0.0f);
		)", GetInput( normalInputSlot ).name, GetOutput( normalOutputSlot ).name );

		/*std::string functionBody =
			"float3 T = normalize( vertexData.tangent.xyz );\n"
			"float3 N = normalize( vertexData.normal.xyz );\n"
			"float3 B = normalize( cross( N, T ) * vertexData.tangent.w );\n"
			"float3x3 TBN = float3x3( T, B, N );\n" +
			std::format( "output.{} = float4(mul(TBN, input.{}.rgb), 0.0f);\n", GetOutput( normalOutputSlot ).name, GetInput( normalInputSlot ).name );*/

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	uint normalInputSlot = 0;
private:
	uint normalOutputSlot = 0;
};

