#pragma once
#include <sstream>
#include <string>
#include <vector>
#include "NodeLibrary.h"
#include "NodeTypes.h"
#include "Types.h"

struct MaterialEdge {
	uint fromNode;      // Index source node
	uint fromSlot;		// Index of output slot
	uint toNode;        // Index destination node
	uint toSlot;		// Index of input slot
};

class MaterialGraph {
public:
	MaterialGraph( std::vector<NodeTypes>& nodes, std::vector<MaterialEdge>& edges )
		: nodes( nodes ), edges( edges )
	{
	}
public:
	std::string GetShaderCode( NodeLibrary& nodeLibrary );
public:
	void AddNode( const NodeTypes& node ) { nodes.push_back( node ); }
	void AddEdge( const MaterialEdge& edge ) { edges.push_back( edge ); }
	const std::vector<NodeTypes>& GetNodes() const { return nodes; }
	const std::vector<MaterialEdge>& GetEdges() const { return edges; }
private:
	std::vector<uint> TopologicalSort() const;
	std::vector<MaterialEdge> GetIncomingEdges( uint nodeIndex ) const;
	std::string GetInputStructs( NodeLibrary& nodeLibrary );
	std::string GetOutputStructs( NodeLibrary& nodeLibrary );
	std::string GetParameterStructs( NodeLibrary& nodeLibrary );
	std::string GetShaderFunctions( NodeLibrary& nodeLibrary );
private:
	std::vector<NodeTypes> nodes;
	std::vector<MaterialEdge> edges;
};