#pragma once
#include <d3dcommon.h>
#include <string>
#include <vector>
#include <wrl\client.h>
#include "Asset.h"
#include "JsonSerializer.h"
#include "NodeLibrary.h"
#include "NodeTypes.h"
#include "SystemManager.h"
#include "Types.h"

using Microsoft::WRL::ComPtr;

class MaterialTemplate :
	public Asset {
public:
	MaterialTemplate( NodeLibrary& nodeLibrary, const std::string& name = "MaterialTemplate" )
		: Asset( AssetType::MaterialTemplate, name ),
		nodeLibrary( nodeLibrary )
	{
	}
	MaterialTemplate( const std::vector<NodeTypes>& nodes,
					  const std::vector<NodeConnection>& edges,
					  NodeLibrary& nodeLibrary,
					  const std::string& name = "MaterialTemplate" )
		: Asset( AssetType::MaterialTemplate, name ),
		nodes( nodes ),
		edges( edges ),
		nodeLibrary( nodeLibrary )
	{
		BuildParameterBindings();
	}
	MaterialTemplate() = default;
	~MaterialTemplate()
	{
		nodes.clear();
		edges.clear();
		parameterBindings.clear();
	}
public:
	void GenerateShaderCode();
	void Load( SystemManager* systemManager, JsonSerializer& serializer ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
public:
	void AddNode( const NodeTypes& node ) { nodes.push_back( node ); }
	void AddEdge( const NodeConnection& edge ) { edges.push_back( edge ); }
	void BuildParameterBindings();
public:
	ComPtr<ID3DBlob>& GetPixelShaderBlob() { return psBlob; };
	const std::string& GetShaderCode() const { return shaderCode; }
	const std::vector<NodeTypes>& GetNodes() const { return nodes; }
	const std::vector<NodeConnection>& GetEdges() const { return edges; }
	const ParameterBinding* GetParameterBinding( uint nodeIndex, uint parameterIndex, NodeParameter parameter ) const;
	std::string GetParameterValue( uint nodeIndex, uint parameterIndex, NodeParameter parameter ) const;
private:
	std::vector<uint> TopologicalSort() const;
	std::vector<NodeConnection> GetIncomingEdges( uint nodeIndex ) const;
	std::vector<uint> GetUnconnectedInputSlots( uint nodeIndex ) const;
	std::string GetInputStructs();
	std::string GetOutputStructs();
	std::string GetParameterStructs();
	std::string GetShaderFunctions();
private:
	std::string shaderCode;
	std::vector<NodeTypes> nodes;
	std::vector<NodeConnection> edges;
	std::vector<ParameterBinding> parameterBindings;
private:
	NodeLibrary& nodeLibrary;
	ComPtr<ID3DBlob> psBlob;
};