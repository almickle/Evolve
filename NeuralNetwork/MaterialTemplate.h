#pragma once
#include <string>
#include <vector>
#include "Asset.h"
#include "GpuResourceManager.h"
#include "JsonSerializer.h"
#include "NodeLibrary.h"
#include "NodeTypes.h"
#include "Types.h"

struct MaterialEdge {
	uint fromNode;      // Index source node
	uint fromSlot;		// Index of output slot
	uint toNode;        // Index destination node
	uint toSlot;		// Index of input slot
};

struct ParameterBinding {
	uint nodeIndex;
	uint parameterIndex;
	NodeParameterTypes parameterType;
	uint cbufferSlot;
};

class MaterialTemplate :
	public Asset {
public:
	MaterialTemplate( NodeLibrary& nodeLibrary, const std::string& name = "MaterialTemplate" )
		: Asset( AssetType::MaterialTemplate, name ),
		nodeLibrary( nodeLibrary )
	{
	}
	MaterialTemplate( const std::vector<NodeTypes>& nodes,
					  const std::vector<MaterialEdge>& edges,
					  NodeLibrary& nodeLibrary,
					  const std::string& name = "MaterialTemplate" )
		: Asset( AssetType::MaterialTemplate, name ),
		nodes( nodes ),
		edges( edges ),
		nodeLibrary( nodeLibrary )
	{
		BuildParameterBindings( nodeLibrary );
	}
	MaterialTemplate() = default;
	~MaterialTemplate()
	{
		nodes.clear();
		edges.clear();
		parameterBindings.clear();
	}
public:
	void GenerateShaderCode( NodeLibrary& nodeLibrary );
	void Load( GpuResourceManager& resourceManager, JsonSerializer& serializer ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
public:
	void AddNode( const NodeTypes& node ) { nodes.push_back( node ); }
	void AddEdge( const MaterialEdge& edge ) { edges.push_back( edge ); }
	void BuildParameterBindings( NodeLibrary& nodeLibrary );
public:
	const std::string& GetShaderCode() const { return shaderCode; }
	const std::vector<NodeTypes>& GetNodes() const { return nodes; }
	const std::vector<MaterialEdge>& GetEdges() const { return edges; }
	const ParameterBinding* GetParameterBinding( uint nodeIndex, uint parameterIndex, NodeParameter parameter ) const;
	std::string GetParameterValue( uint nodeIndex, uint parameterIndex, NodeParameter parameter ) const;
private:
	std::vector<uint> TopologicalSort() const;
	std::vector<MaterialEdge> GetIncomingEdges( uint nodeIndex ) const;
	std::string GetInputStructs( NodeLibrary& nodeLibrary );
	std::string GetOutputStructs( NodeLibrary& nodeLibrary );
	std::string GetParameterStructs( NodeLibrary& nodeLibrary );
	std::string GetShaderFunctions( NodeLibrary& nodeLibrary );
private:
	std::string shaderCode;
	std::vector<NodeTypes> nodes;
	std::vector<MaterialEdge> edges;
	std::vector<ParameterBinding> parameterBindings;
private:
	NodeLibrary& nodeLibrary;
};