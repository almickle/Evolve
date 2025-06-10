#include <algorithm>
#include <format>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "MaterialGraph.h"
#include "NodeLibrary.h"
#include "Types.h"

std::vector<uint> MaterialGraph::TopologicalSort() const
{
	std::unordered_map<uint, std::vector<uint>> dependencies; // node -> list of nodes it depends on
	std::unordered_map<uint, int> inDegree; // node -> number of incoming edges

	// Initialize
	for( uint i = 0; i < nodes.size(); ++i ) {
		inDegree[i] = 0;
	}

	// Build dependency graph
	for( const auto& edge : edges ) {
		dependencies[edge.fromNode].push_back( edge.toNode );
		inDegree[edge.toNode]++;
	}

	// Start with nodes that have no incoming edges
	std::queue<uint> q;
	for( const auto& [node, degree] : inDegree ) {
		if( degree == 0 ) q.push( node );
	}

	std::vector<uint> sorted;
	while( !q.empty() ) {
		uint node = q.front(); q.pop();
		sorted.push_back( node );
		for( uint dep : dependencies[node] ) {
			if( --inDegree[dep] == 0 ) {
				q.push( dep );
			}
		}
	}

	// Optionally, reverse to process from root backwards
	std::reverse( sorted.begin(), sorted.end() );
	return sorted;
}

std::string MaterialGraph::GetShaderCode( NodeLibrary& nodeLibrary )
{
	std::ostringstream oss;

	auto inputStructs = GetInputStructs( nodeLibrary );
	auto outputStructs = GetOutputStructs( nodeLibrary );
	auto parameterStructs = GetParameterStructs( nodeLibrary );
	auto shaderFunctions = GetShaderFunctions( nodeLibrary );
	std::string signature = "float4 main(VSOutput input) : SV_TARGET";

	oss << inputStructs << outputStructs << parameterStructs << shaderFunctions << signature << "\n" << "{";

	std::vector<uint> sorted = TopologicalSort();
	std::vector<std::string> shaderChunks;
	shaderChunks.reserve( sorted.size() );
	for( auto& index : sorted )
	{
		std::ostringstream shaderChunk;

		auto node = nodes[index];
		auto nodeRef = nodeLibrary.GetNode( node );

		shaderChunk << nodeRef->GetInputStatement( index ) << "\n";
		shaderChunk << nodeRef->GetParameterStatement( index ) << "\n";

		auto incomingEdges = GetIncomingEdges( index );
		for( auto& incomingEdge : incomingEdges )
		{
			auto incomingNode = nodes[incomingEdge.fromNode];
			auto incomingNodeRef = nodeLibrary.GetNode( incomingNode );
			shaderChunk
				<< nodeRef->GetInputDataName( index )
				<< "."
				<< nodeRef->GetInputSlotName( incomingEdge.toSlot )
				<< " = "
				<< incomingNodeRef->GetOutputDataName( incomingEdge.fromNode )
				<< "."
				<< incomingNodeRef->GetOutputSlotName( incomingEdge.fromSlot )
				<< ";\n";
		}
		for( uint i = 0; i < nodeRef->GetParameterCount(); ++i )
		{
			shaderChunk
				<< nodeRef->GetParameterDataName( index )
				<< "."
				<< nodeRef->GetParameterSlotName( i )
				<< " = "
				<< std::to_string( 0 ) // Placeholder for actual parameter value, replace with actual logic to get parameter value
				<< ";\n";
		}

		shaderChunk << nodeRef->GetOutputStatement( index );
		shaderChunks.push_back( shaderChunk.str() );
	}

	auto root = nodeLibrary.GetNode( nodes[0] );
	std::string outputStatement = std::format( "return {}.{};", root->GetOutputDataName( 0 ), root->GetOutputs()[0].name );

	for( auto it = shaderChunks.rbegin(); it != shaderChunks.rend(); ++it ) {
		oss << *it << "\n";
	}

	oss << outputStatement << "\n};";

	return oss.str();
}

std::vector<MaterialEdge> MaterialGraph::GetIncomingEdges( uint nodeIndex ) const
{
	std::vector<MaterialEdge> result;
	for( const auto& edge : edges )
	{
		if( edge.toNode == nodeIndex )
			result.push_back( edge );
	}
	return result;
}
std::string MaterialGraph::GetInputStructs( NodeLibrary& nodeLibrary )
{
	std::ostringstream oss;
	for( const auto& node : nodes ) {
		oss << nodeLibrary.GetNode( node )->GetInputStruct() << "\n";
	}
	return oss.str();
}
std::string MaterialGraph::GetOutputStructs( NodeLibrary& nodeLibrary )
{
	std::ostringstream oss;
	for( const auto& node : nodes ) {
		oss << nodeLibrary.GetNode( node )->GetOutputStruct() << "\n";
	}
	return oss.str();
}
std::string MaterialGraph::GetParameterStructs( NodeLibrary& nodeLibrary )
{
	std::ostringstream oss;
	for( const auto& node : nodes ) {
		oss << nodeLibrary.GetNode( node )->GetParameterStruct() << "\n";
	}
	return oss.str();
}
std::string MaterialGraph::GetShaderFunctions( NodeLibrary& nodeLibrary )
{
	std::ostringstream oss;
	for( const auto& node : nodes ) {
		oss << nodeLibrary.GetNode( node )->GetShaderFunction() << "\n";
	}
	return oss.str();
}