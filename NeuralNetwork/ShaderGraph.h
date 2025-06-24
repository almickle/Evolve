#pragma once
#include <d3dcommon.h>
#include <string>
#include <vector>
#include <wrl\client.h>
#include "Asset.h"
#include "JsonSerializer.h"
#include "NodeLibrary.h"
#include "NodeTypes.h"
#include "Renderer.h"
#include "SystemManager.h"
#include "Types.h"

using Microsoft::WRL::ComPtr;

class ShaderGraph : public Asset
{
protected:
	ShaderGraph( NodeLibrary& nodeLibrary, const AssetType& assetType, const ShaderType& shaderType, const std::string& name )
		: Asset( assetType, name ),
		nodeLibrary( nodeLibrary ),
		shaderType( shaderType )
	{
	}
	virtual ~ShaderGraph() {};
public:
	void Load( SystemManager* systemManager, JsonSerializer& serializer ) override {};
	std::string Serialize( JsonSerializer& serializer ) const override { return ""; };
	void Deserialize( JsonSerializer& serializer ) override {};
public:
	void AddNode( const NodeTypes& node ) { nodes.push_back( node ); }
	void AddEdge( const NodeConnection& edge ) { edges.push_back( edge ); }
public:
	ComPtr<ID3DBlob>& GetShaderBlob() { return blob; };
	const std::vector<NodeTypes>& GetNodes() const { return nodes; }
	const std::vector<NodeConnection>& GetEdges() const { return edges; }
protected:
	void Compile( Renderer& renderer );
private:
	void GenerateShaderCode();
private:
	std::vector<uint> TopologicalSort() const;
	std::vector<NodeConnection> GetIncomingEdges( uint nodeIndex ) const;
	std::vector<uint> GetUnconnectedInputSlots( uint nodeIndex ) const;
	std::string GetInputStructs();
	std::string GetOutputStructs();
	std::string GetParameterStructs();
	std::string GetShaderFunctions();
	std::string GetParameterValue( uint nodeIndex, uint parameterIndex, NodeParameter parameter ) const;
private:
	std::vector<NodeTypes> nodes;
	std::vector<NodeConnection> edges;
	std::vector<ParameterBinding> parameterBindings;
private:
	ComPtr<ID3DBlob> blob;
	ShaderType shaderType;
	std::string shaderCode;
	std::string signature;
private:
	NodeLibrary& nodeLibrary;
};