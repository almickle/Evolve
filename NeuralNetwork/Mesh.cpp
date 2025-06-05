#include <cstdint>
#include <d3d12.h>
#include <string>
#include <vector>
#include <wrl\client.h>
#include "DataStructures.h"
#include "GpuResourceManager.h"
#include "IndexBuffer.h"
#include "Mesh.h"
#include "VertexBuffer.h"

using Microsoft::WRL::ComPtr;
using uint = unsigned int;

void Mesh::Create( GpuResourceManager& manager, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices )
{
	vertexBufferID = manager.CreateVertexBuffer( vertices, name + "VertexBuffer" );
	indexBufferID = manager.CreateIndexBuffer( indices, name + "IndexBuffer" );
}

void Mesh::Bind( GpuResourceManager& manager, ID3D12GraphicsCommandList* cmdList ) const
{
	auto vertexBuffer = GetVertexBuffer( manager );
	auto indexBuffer = GetIndexBuffer( manager );

	cmdList->IASetVertexBuffers( 0, 1, &vertexBuffer->GetView() );
	cmdList->IASetIndexBuffer( &indexBuffer->GetView() );
	cmdList->IASetPrimitiveTopology( topology );
}

VertexBuffer* Mesh::GetVertexBuffer( GpuResourceManager& manager ) const
{
	return static_cast<VertexBuffer*>(manager.GetResource( vertexBufferID ));
}

IndexBuffer* Mesh::GetIndexBuffer( GpuResourceManager& manager ) const
{
	return static_cast<IndexBuffer*>(manager.GetResource( indexBufferID ));
}

const std::vector<Vertex>& Mesh::GetVertices( GpuResourceManager& manager ) const
{
	auto vertexBuffer = GetVertexBuffer( manager );
	return static_cast<VertexBuffer*>(vertexBuffer)->GetVertices();
}

const std::vector<uint32_t>& Mesh::GetIndices( GpuResourceManager& manager ) const
{
	auto indexBuffer = GetIndexBuffer( manager );
	return static_cast<IndexBuffer*>(indexBuffer)->GetIndices();
}

uint Mesh::GetIndexCount( GpuResourceManager& manager ) const
{
	auto indexBuffer = GetIndexBuffer( manager );
	return static_cast<IndexBuffer*>(indexBuffer)->GetIndexCount();
}