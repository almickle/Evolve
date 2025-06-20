#pragma once
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include "AddNode.h"
#include "AndNode.h"
#include "DisplacementNode.h"
#include "DivideNode.h"
#include "EmitTriangleNode.h"
#include "EmitVertexNode.h"
#include "EqualNode.h"
#include "GreaterThanNode.h"
#include "GreaterThanOrEqualNode.h"
#include "LessThanNode.h"
#include "LessThanOrEqualNode.h"
#include "MaterialOutputNode.h"
#include "MeshPrimitiveNode.h"
#include "MultiplyNode.h"
#include "NegateNode.h"
#include "NodeTypes.h"
#include "NormalMapNode.h"
#include "NotEqualNode.h"
#include "OrNode.h"
#include "PowerNode.h"
#include "RadicalNode.h"
#include "ReciprocalNode.h"
#include "ScalarParameterNode.h"
#include "ShaderNode.h"
#include "SignNode.h"
#include "System.h"
#include "TextureSamplerNode.h"
#include "ThreadInfoNode.h"
#include "VectorBreakNode.h"
#include "VectorMakeNode.h"
#include "VectorScaleNode.h"
#include "VertexDataNode.h"


class NodeLibrary : public System {
public:
	NodeLibrary()
	{
		nodes.reserve( 31 ); // Reserve space for expected node types
		RegisterNode( NodeTypes::MaterialOutput, std::make_unique<MaterialOutputNode>() );
		RegisterNode( NodeTypes::TextureSampler, std::make_unique<TextureSamplerNode>() );
		RegisterNode( NodeTypes::VertexData, std::make_unique<VertexDataNode>() );
		RegisterNode( NodeTypes::VectorScale, std::make_unique<VectorScaleNode>() );
		RegisterNode( NodeTypes::NormalMap, std::make_unique<NormalMapNode>() );
		RegisterNode( NodeTypes::VectorBreak, std::make_unique<VectorBreakNode>() );
		RegisterNode( NodeTypes::ScalarParameter, std::make_unique<ScalarParameterNode>() );
		RegisterNode( NodeTypes::VectorMakeNode, std::make_unique<VectorMakeNode>() );

		RegisterNode( NodeTypes::Add, std::make_unique<AddNode>() );
		RegisterNode( NodeTypes::Subtract, std::make_unique<OrNode>() );
		RegisterNode( NodeTypes::Multiply, std::make_unique<MultiplyNode>() );
		RegisterNode( NodeTypes::Divide, std::make_unique<DivideNode>() );
		RegisterNode( NodeTypes::Power, std::make_unique<PowerNode>() );
		RegisterNode( NodeTypes::Radical, std::make_unique<RadicalNode>() );
		RegisterNode( NodeTypes::Reciprocal, std::make_unique<ReciprocalNode>() );
		RegisterNode( NodeTypes::Negate, std::make_unique<NegateNode>() );
		RegisterNode( NodeTypes::Sign, std::make_unique<SignNode>() );

		RegisterNode( NodeTypes::And, std::make_unique<AndNode>() );
		RegisterNode( NodeTypes::Or, std::make_unique<OrNode>() );
		RegisterNode( NodeTypes::GreaterThan, std::make_unique<GreaterThanNode>() );
		RegisterNode( NodeTypes::GreaterThanOrEqual, std::make_unique<GreaterThanOrEqualNode>() );
		RegisterNode( NodeTypes::LessThan, std::make_unique<LessThanNode>() );
		RegisterNode( NodeTypes::LessThanOrEqual, std::make_unique<LessThanOrEqualNode>() );
		RegisterNode( NodeTypes::Equal, std::make_unique<EqualNode>() );
		RegisterNode( NodeTypes::NotEqual, std::make_unique<NotEqualNode>() );

		RegisterNode( NodeTypes::Displacement, std::make_unique<DisplacementNode>() );
		RegisterNode( NodeTypes::EmitTriangle, std::make_unique<EmitTriangleNode>() );
		RegisterNode( NodeTypes::EmitVertex, std::make_unique<EmitVertexNode>() );
		RegisterNode( NodeTypes::ThreadInfo, std::make_unique<ThreadInfoNode>() );
		RegisterNode( NodeTypes::MeshPrimitivePlane, std::make_unique<MeshPlanePrimitiveNode>() );
	}

	// Register a node type with a node instance
	void RegisterNode( const NodeTypes& typeName, std::unique_ptr<ShaderNode> node )
	{
		nodes[typeName] = std::move( node );
	}

	// Get a node by type name (returns nullptr if not found)
	ShaderNode* GetNode( const NodeTypes& typeName ) const
	{
		auto it = nodes.find( typeName );
		if( it != nodes.end() ) {
			return it->second.get();
		}
		return nullptr;
	}

	// Get all registered node type names
	std::vector<NodeTypes> GetRegisteredTypes() const
	{
		std::vector<NodeTypes> types;
		for( const auto& pair : nodes ) {
			types.push_back( pair.first );
		}
		return types;
	}

private:
	std::unordered_map<NodeTypes, std::unique_ptr<ShaderNode>> nodes;
};