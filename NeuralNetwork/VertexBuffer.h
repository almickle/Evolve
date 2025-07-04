#pragma once
#include <d3d12.h>
#include <string>
#include <vector>
#include "DataStructures.h"
#include "GpuResource.h"
#include "Types.h"

class VertexBuffer : public GpuResource {
public:
	VertexBuffer( const std::vector<Vertex>& vertices, const std::string& name = "VertexBuffer" )
		: GpuResource( D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, name ),
		vertexCount( static_cast<uint>(vertices.size()) ),
		vertexStride( sizeof( Vertex ) ),
		vertices( vertices )
	{
		size = vertices.size() * sizeof( Vertex );
	}
	~VertexBuffer()
	{
		vertices.clear();
	}
public:
	D3D12_VERTEX_BUFFER_VIEW* GetView() { return &vbView; }
	uint GetVertexCount() const { return vertexCount; }
	uint GetVertexStride() const { return vertexStride; }
	const std::vector<Vertex>& GetVertices() const { return vertices; }
	void* GetData() override { return vertices.data(); }
public:
	void SetVertexBufferView( const D3D12_VERTEX_BUFFER_VIEW& view ) { vbView = view; }
private:
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	uint vertexCount = 0;
	uint vertexStride = 0;
	std::vector<Vertex> vertices;
};