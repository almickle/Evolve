#pragma once
#include <cstdint>
#include <d3d12.h>
#include <d3dcommon.h>
#include <string>
#include <vector>
#include "DataStructures.h"
#include "IndexBuffer.h"
#include "Types.h"
#include "VertexBuffer.h"

class Renderer;
class GpuResourceManager;

class Mesh {
public:
	Mesh( std::string name ) : name( name ) {};
	~Mesh() = default;
public:
	void Create( GpuResourceManager& manager, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices );
	void Bind( GpuResourceManager& manager, ID3D12GraphicsCommandList* cmdList ) const;
public:
	const std::vector<Vertex>& GetVertices( GpuResourceManager& manager ) const;
	const std::vector<uint32_t>& GetIndices( GpuResourceManager& manager ) const;
	uint GetIndexCount( GpuResourceManager& manager ) const;
	VertexBuffer* GetVertexBuffer( GpuResourceManager& manager ) const;
	IndexBuffer* GetIndexBuffer( GpuResourceManager& manager ) const;
private:
	std::string name;
	ResourceID vertexBufferID;
	ResourceID indexBufferID;
	D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};