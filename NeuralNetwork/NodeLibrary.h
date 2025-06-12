#pragma once
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include "MaterialNode.h"
#include "MaterialOutputNode.h"
#include "NodeTypes.h"
#include "NormalMapNode.h"
#include "ScalarParameterNode.h"
#include "TextureSamplerNode.h"
#include "VectorBreakNode.h"
#include "VectorScaleNode.h"
#include "VertexDataNode.h"

class NodeLibrary {
public:
	NodeLibrary()
	{
		nodes.reserve( 7 ); // Reserve space for expected node types
		RegisterNode( NodeTypes::MaterialOutput, std::make_unique<MaterialOutputNode>() );
		RegisterNode( NodeTypes::TextureSampler, std::make_unique<TextureSamplerNode>() );
		RegisterNode( NodeTypes::VertexData, std::make_unique<VertexDataNode>() );
		RegisterNode( NodeTypes::VectorScale, std::make_unique<VectorScaleNode>() );
		RegisterNode( NodeTypes::NormalMap, std::make_unique<NormalMapNode>() );
		RegisterNode( NodeTypes::VectorBreak, std::make_unique<VectorBreakNode>() );
		RegisterNode( NodeTypes::ScalarParameter, std::make_unique<ScalarParameterNode>() );
	}

	// Register a node type with a node instance
	void RegisterNode( const NodeTypes& typeName, std::unique_ptr<MaterialNode> node )
	{
		nodes[typeName] = std::move( node );
	}

	// Get a node by type name (returns nullptr if not found)
	MaterialNode* GetNode( const NodeTypes& typeName ) const
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
	std::unordered_map<NodeTypes, std::unique_ptr<MaterialNode>> nodes;
};