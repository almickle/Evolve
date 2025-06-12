#include <algorithm>
#include <format>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "Asset.h"
#include "GpuResourceManager.h"
#include "JsonSerializer.h"
#include "MaterialNode.h"
#include "MaterialTemplate.h"
#include "NodeLibrary.h"
#include "NodeTypes.h"
#include "Types.h"

std::string MaterialTemplate::Serialize( JsonSerializer& serializer ) const
{
	serializer.StartDocument();
	serializer.BeginObject();

	// Meta data and asset ids
	SerializeBaseAsset( serializer );

	// Nodes
	serializer.WriteArray( "nodes", nodes );

	// Edges
	serializer.BeginArray( "edges" );
	for( const auto& edge : edges ) {
		serializer.BeginObject();
		serializer.Write( "fromNode", edge.fromNode );
		serializer.Write( "fromSlot", edge.fromSlot );
		serializer.Write( "toNode", edge.toNode );
		serializer.Write( "toSlot", edge.toSlot );
		serializer.EndObject();
	}
	serializer.EndArray();

	// Parameter Bindings
	serializer.BeginArray( "parameterBindings" );
	for( const auto& binding : parameterBindings ) {
		serializer.BeginObject();
		serializer.Write( "nodeIndex", binding.nodeIndex );
		serializer.Write( "parameterIndex", binding.parameterIndex );
		serializer.Write( "parameterType", static_cast<int>(binding.parameterType) );
		serializer.Write( "cbufferSlot", binding.cbufferSlot );
		serializer.EndObject();
	}
	serializer.EndArray();

	serializer.EndObject();
	serializer.EndDocument();

	return serializer.GetString();
}

void MaterialTemplate::Load( GpuResourceManager& resourceManager, JsonSerializer& serializer )
{
	Deserialize( serializer );
}

void MaterialTemplate::Deserialize( JsonSerializer& serializer )
{
	// Base asset fields (id, name, type, assetIds, etc.)
	DeserializeBaseAsset( serializer );

	// Nodes
	nodes = serializer.ReadArray<NodeTypes>( "nodes" );

	// Edges
	edges.clear();
	auto edgeArray = serializer.GetSubObject( "edges" );
	for( const auto& e : edgeArray ) {
		MaterialEdge edge{};
		edge.fromNode = e.at( "fromNode" ).get<uint>();
		edge.fromSlot = e.at( "fromSlot" ).get<uint>();
		edge.toNode = e.at( "toNode" ).get<uint>();
		edge.toSlot = e.at( "toSlot" ).get<uint>();
		edges.push_back( edge );
	}

	// Parameter Bindings
	parameterBindings.clear();
	auto paramArray = serializer.GetSubObject( "parameterBindings" );
	for( const auto& p : paramArray ) {
		ParameterBinding binding{};
		binding.nodeIndex = p.at( "nodeIndex" ).get<uint>();
		binding.parameterIndex = p.at( "parameterIndex" ).get<uint>();
		binding.parameterType = static_cast<NodeParameterTypes>(p.at( "parameterType" ).get<int>());
		binding.cbufferSlot = p.at( "cbufferSlot" ).get<uint>();
		parameterBindings.push_back( binding );
	}
}


void MaterialTemplate::BuildParameterBindings( NodeLibrary& nodeLibrary )
{
	parameterBindings.clear();
	uint texturebufferSlot = 0;
	uint vectorbufferSlot = 0;
	uint scalarbufferSlot = 0;
	for( uint nodeIdx = 0; nodeIdx < nodes.size(); ++nodeIdx ) {
		MaterialNode* node = nodeLibrary.GetNode( nodes[nodeIdx] );
		uint paramCount = node->GetParameterCount();
		auto& params = node->GetParameters();
		for( uint paramIdx = 0; paramIdx < paramCount; ++paramIdx ) {
			NodeParameter param = params[paramIdx];
			switch( param.type )
			{
				case NodeParameterTypes::Texture:
					parameterBindings.push_back( { nodeIdx, paramIdx, param.type, texturebufferSlot++ } );
					break;
				case NodeParameterTypes::Vector:
					parameterBindings.push_back( { nodeIdx, paramIdx, param.type, vectorbufferSlot++ } );
					break;
				case NodeParameterTypes::Scalar:
					parameterBindings.push_back( { nodeIdx, paramIdx, param.type, scalarbufferSlot++ } );
					break;
			}
		}
	}
}

const ParameterBinding* MaterialTemplate::GetParameterBinding( uint nodeIndex, uint parameterIndex, NodeParameter parameter ) const
{
	for( const auto& binding : parameterBindings ) {
		if( binding.nodeIndex == nodeIndex && binding.parameterIndex == parameterIndex && binding.parameterType == parameter.type )
			return &binding;
	}
	return nullptr;
}

std::string MaterialTemplate::GetParameterValue( uint nodeIndex, uint parameterIndex, NodeParameter parameter ) const
{
	for( const auto& binding : parameterBindings ) {
		if( binding.nodeIndex == nodeIndex && binding.parameterIndex == parameterIndex )
			switch( binding.parameterType )
			{
				case NodeParameterTypes::Texture:
					return std::format( "textureSlots[{}]", binding.cbufferSlot );
				case NodeParameterTypes::Vector:
					return std::format( "vectorSlots[{}]", binding.cbufferSlot );
				case NodeParameterTypes::Scalar:
					return std::format( "scalarSlots[{}]", binding.cbufferSlot );
				default:
					return "/* invalid parameter type */";
			}
	}
}

std::vector<uint> MaterialTemplate::TopologicalSort() const
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

std::string MaterialTemplate::GetShaderCode( NodeLibrary& nodeLibrary )
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
	for( auto& nodeIndex : sorted )
	{
		std::ostringstream shaderChunk;

		auto node = nodes[nodeIndex];
		auto nodeRef = nodeLibrary.GetNode( node );

		shaderChunk << nodeRef->GetInputStatement( nodeIndex ) << "\n";
		shaderChunk << nodeRef->GetParameterStatement( nodeIndex ) << "\n";

		auto incomingEdges = GetIncomingEdges( nodeIndex );
		for( auto& incomingEdge : incomingEdges )
		{
			auto incomingNode = nodes[incomingEdge.fromNode];
			auto incomingNodeRef = nodeLibrary.GetNode( incomingNode );
			shaderChunk
				<< nodeRef->GetInputDataName( nodeIndex )
				<< "."
				<< nodeRef->GetInputSlotName( incomingEdge.toSlot )
				<< " = "
				<< incomingNodeRef->GetOutputDataName( incomingEdge.fromNode )
				<< "."
				<< incomingNodeRef->GetOutputSlotName( incomingEdge.fromSlot )
				<< ";\n";
		}
		for( uint parameterIndex = 0; parameterIndex < nodeRef->GetParameterCount(); ++parameterIndex )
		{
			shaderChunk
				<< nodeRef->GetParameterDataName( nodeIndex )
				<< "."
				<< nodeRef->GetParameterSlotName( parameterIndex )
				<< " = "
				<< GetParameterValue( nodeIndex, parameterIndex, nodeRef->GetParameter( parameterIndex ) )
				<< ";\n";
		}

		shaderChunk << nodeRef->GetOutputStatement( nodeIndex );
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

std::vector<MaterialEdge> MaterialTemplate::GetIncomingEdges( uint nodeIndex ) const
{
	std::vector<MaterialEdge> result;
	for( const auto& edge : edges )
	{
		if( edge.toNode == nodeIndex )
			result.push_back( edge );
	}
	return result;
}
std::string MaterialTemplate::GetInputStructs( NodeLibrary& nodeLibrary )
{
	std::ostringstream oss;
	for( const auto& node : nodes ) {
		oss << nodeLibrary.GetNode( node )->GetInputStruct() << "\n";
	}
	return oss.str();
}
std::string MaterialTemplate::GetOutputStructs( NodeLibrary& nodeLibrary )
{
	std::ostringstream oss;
	for( const auto& node : nodes ) {
		oss << nodeLibrary.GetNode( node )->GetOutputStruct() << "\n";
	}
	return oss.str();
}
std::string MaterialTemplate::GetParameterStructs( NodeLibrary& nodeLibrary )
{
	std::ostringstream oss;
	for( const auto& node : nodes ) {
		oss << nodeLibrary.GetNode( node )->GetParameterStruct() << "\n";
	}
	return oss.str();
}
std::string MaterialTemplate::GetShaderFunctions( NodeLibrary& nodeLibrary )
{
	std::ostringstream oss;
	for( const auto& node : nodes ) {
		oss << nodeLibrary.GetNode( node )->GetShaderFunction() << "\n";
	}
	return oss.str();
}