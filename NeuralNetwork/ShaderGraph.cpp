#include <algorithm>
#include <exception>
#include <format>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "JsonSerializer.h"
#include "NodeTypes.h"
#include "Renderer.h"
#include "ShaderGraph.h"
#include "SystemManager.h"
#include "Types.h"

void  ShaderGraph::Load( SystemManager* systemManager, JsonSerializer& serializer )
{
	Deserialize( serializer );
};

std::string ShaderGraph::Serialize( JsonSerializer& serializer ) const
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
};

void  ShaderGraph::Deserialize( JsonSerializer& serializer )
{
	try
	{
		// Base asset fields (id, name, type, assetIds, etc.)
		DeserializeBaseAsset( serializer );

		// Nodes
		nodes = serializer.ReadArray<NodeTypes>( "nodes" );

		// Edges
		edges.clear();
		auto edgeArray = serializer.GetSubObject( "edges" );
		for( const auto& e : edgeArray ) {
			NodeConnection edge{};
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
			binding.parameterType = static_cast<NodeParameterType>(p.at( "parameterType" ).get<int>());
			binding.cbufferSlot = p.at( "cbufferSlot" ).get<uint>();
			parameterBindings.push_back( binding );
		}
	}
	catch( const std::exception& )
	{
		throw std::runtime_error( "Failed to deserialize MaterialTemplate" );
	}
};

void ShaderGraph::Compile( Renderer& renderer )
{
	GenerateShaderCode();
	renderer.CompileShader( shaderCode, shaderType, name, blob );
}

void ShaderGraph::GenerateShaderCode()
{
	std::ostringstream oss;

	for( const auto& include : includes )
	{
		oss << "#include \"" << include << "\"\n";
	}
	auto inputStructs = GetInputStructs();
	auto outputStructs = GetOutputStructs();
	auto parameterStructs = GetParameterStructs();
	auto shaderFunctions = GetShaderFunctions();

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
		for( auto& unconnectedSlot : GetUnconnectedInputSlots( nodeIndex ) ) {
			shaderChunk
				<< nodeRef->GetInputDataName( nodeIndex )
				<< "."
				<< nodeRef->GetInputSlotName( unconnectedSlot )
				<< " = "
				<< nodeRef->GetInput( unconnectedSlot ).GetHlslValue()
				<< ";\n";
		}
		shaderChunk << nodeRef->GetParameterStatement( nodeIndex ) << "\n";
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

	shaderCode = oss.str();
}

std::vector<uint> ShaderGraph::TopologicalSort() const
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

std::vector<uint> ShaderGraph::GetUnconnectedInputSlots( uint nodeIndex ) const
{
	// 1. Get the node reference
	auto nodeType = nodes[nodeIndex];
	auto nodeRef = nodeLibrary.GetNode( nodeType );

	// 2. Get the total number of input slots
	uint inputCount = nodeRef->GetInputCount();

	// 3. Collect connected input slots
	std::set<uint> connectedSlots;
	for( const auto& edge : edges )
	{
		if( edge.toNode == nodeIndex )
			connectedSlots.insert( edge.toSlot );
	}

	// 4. Find unconnected input slots
	std::vector<uint> unconnectedSlots;
	for( uint i = 0; i < inputCount; ++i )
	{
		if( connectedSlots.find( i ) == connectedSlots.end() )
			unconnectedSlots.push_back( i );
	}

	return unconnectedSlots;
}

std::vector<NodeConnection> ShaderGraph::GetIncomingEdges( uint nodeIndex ) const
{
	std::vector<NodeConnection> result;
	for( const auto& edge : edges )
	{
		if( edge.toNode == nodeIndex )
			result.push_back( edge );
	}
	return result;
}
std::string ShaderGraph::GetInputStructs()
{
	std::ostringstream oss;
	std::set<NodeTypes> recordedTypes;
	for( const auto& node : nodes ) {
		if( recordedTypes.find( node ) != recordedTypes.end() )
			continue;
		oss << nodeLibrary.GetNode( node )->GetInputStruct() << "\n";
		recordedTypes.insert( node );
	}
	return oss.str();
}
std::string ShaderGraph::GetOutputStructs()
{
	std::ostringstream oss;
	std::set<NodeTypes> recordedTypes;
	for( const auto& node : nodes ) {
		if( recordedTypes.find( node ) != recordedTypes.end() )
			continue;
		oss << nodeLibrary.GetNode( node )->GetOutputStruct() << "\n";
		recordedTypes.insert( node );
	}
	return oss.str();
}
std::string ShaderGraph::GetParameterStructs()
{
	std::ostringstream oss;
	std::set<NodeTypes> recordedTypes;
	for( const auto& node : nodes ) {
		if( recordedTypes.find( node ) != recordedTypes.end() )
			continue;
		oss << nodeLibrary.GetNode( node )->GetParameterStruct() << "\n";
		recordedTypes.insert( node );
	}
	return oss.str();
}
std::string ShaderGraph::GetShaderFunctions()
{
	std::ostringstream oss;
	std::set<NodeTypes> recordedTypes;
	for( const auto& node : nodes ) {
		if( recordedTypes.find( node ) != recordedTypes.end() )
			continue;
		oss << nodeLibrary.GetNode( node )->GetShaderFunction() << "\n";
		recordedTypes.insert( node );
	}
	return oss.str();
}

std::string ShaderGraph::GetParameterValue( uint nodeIndex, uint parameterIndex, NodeParameter parameter ) const
{
	for( const auto& binding : parameterBindings ) {
		if( binding.nodeIndex == nodeIndex && binding.parameterIndex == parameterIndex ) {
			switch( binding.parameterType )
			{
				case NodeParameterType::Texture:
					return std::format( "GetTextureSlot({})", binding.cbufferSlot );
				case NodeParameterType::Vector:
					return std::format( "vectorSlots[{}]", binding.cbufferSlot );
				case NodeParameterType::Scalar:
					return std::format( "scalarSlots[{}]", binding.cbufferSlot );
				default:
					return "/* invalid parameter type */";
			}
		}
	}
	return "/* parameter not found */";
}