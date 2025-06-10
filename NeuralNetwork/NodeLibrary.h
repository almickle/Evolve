#pragma once
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include "MaterialNode.h"
#include "MaterialOutputNode.h"
#include "NodeTypes.h"
#include "TextureSamplerNode.h"

class NodeLibrary {
public:
	NodeLibrary()
	{
		nodes.reserve( 2 ); // Reserve space for expected node types
		RegisterNode( NodeTypes::TextureSampler, std::make_unique<TextureSamplerNode>() );
		RegisterNode( NodeTypes::MaterialOutput, std::make_unique<MaterialOutputNode>() );
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