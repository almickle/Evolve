#pragma once
#include <d3d12.h>
#include <string>
#include <vector>
#include "Asset.h"
#include "DataStructures.h"
#include "GpuResourceManager.h"
#include "IndexBuffer.h"
#include "JsonSerializer.h"
#include "SystemManager.h"
#include "Types.h"
#include "VertexBuffer.h"

struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<uint> indices;
};

class Mesh : public Asset {
public:
	Mesh( const std::string& name = "Mesh" )
		: Asset( AssetType::Mesh, name )
	{
	}
	Mesh( const std::vector<MeshData>& meshData, const std::string& name = "Mesh" )
		: Asset( AssetType::Mesh, name ),
		meshData( meshData )
	{
	}
	~Mesh() = default;
public:
	void Load( SystemManager* systemManager ) override;
	virtual std::string Serialize( JsonSerializer& serializer ) const override;
	virtual void Deserialize( JsonSerializer& serializer ) override;
public:
	std::vector<D3D12_VERTEX_BUFFER_VIEW*> GetVertexBufferViews( GpuResourceManager& resourceManager ) const
	{
		std::vector<D3D12_VERTEX_BUFFER_VIEW*> views;
		views.reserve( meshData.size() );
		for( const auto& subMesh : subassets ) {
			auto* vb = static_cast<VertexBuffer*>(resourceManager.GetResource( subMesh->GetResourceIDs()[0] ));
			views.push_back( vb->GetView() );
		}
		return views;
	}
	std::vector<D3D12_INDEX_BUFFER_VIEW*> GetIndexBufferViews( GpuResourceManager& resourceManager ) const
	{
		std::vector<D3D12_INDEX_BUFFER_VIEW*> views;
		views.reserve( meshData.size() );
		for( const auto& subMesh : subassets ) {
			auto* ib = static_cast<IndexBuffer*>(resourceManager.GetResource( subMesh->GetResourceIDs()[1] ));
			views.push_back( ib->GetView() );
		}
		return views;
	}
private:
	std::vector<MeshData> meshData;
};