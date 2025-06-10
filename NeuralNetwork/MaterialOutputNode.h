#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "MaterialNode.h"
#include "NodeTypes.h"

class MaterialOutputNode : public MaterialNode {
public:
	MaterialOutputNode( const std::string& name = "MaterialOutputNode" )
		: MaterialNode( 7, 1, 0, name )
	{
		AddInput( 0, NodeSlot{ "baseColor", DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f} } );
		AddInput( 1, NodeSlot{ "metallic", 0.0f } );
		AddInput( 2, NodeSlot{ "roughness", 1.0f } );
		AddInput( 3, NodeSlot{ "normal", DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f} } );
		AddInput( 4, NodeSlot{ "emissive", DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f} } );
		AddInput( 5, NodeSlot{ "opacity", 1.0f } );
		AddInput( 6, NodeSlot{ "ambientOcculusion", 1.0f } );
		AddOutput( 0, NodeSlot{ "color", DirectX::XMFLOAT4{1.0f, 1.0f, 1.0f, 1.0f} } );
	}
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = "";

		std::string shaderFunction = std::format( "{}\n{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
};