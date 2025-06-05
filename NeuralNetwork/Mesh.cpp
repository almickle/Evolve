//#include <cstdint>
//#include <d3d12.h>
//#include <string>
//#include <vector>
//#include <Windows.h>
//#include <wrl\client.h>
//#include "Mesh.h"
//#include "Renderer.h"
//
//using Microsoft::WRL::ComPtr;
//
//Mesh::Mesh( const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& name )
//{
//	this->name = name;
//	this->vertices = vertices;
//	this->indices = indices;
//	this->indexCount = static_cast<uint>(indices.size());
//};
//
//void Mesh::CreateGPUResources( Renderer& renderer )
//{
//	// Vertex Buffer
//	vertexBuffer.CreateResource( renderer, static_cast<uint>(vertices.size()), sizeof( Vertex ) );
//	vertexBuffer.Upload( renderer, vertices.data() );
//
//	// Index Buffer
//	indexBuffer.CreateResource( renderer, static_cast<uint>(indices.size()), DXGI_FORMAT_R32_uint );
//	indexBuffer.Upload( renderer, indices.data() );
//}
//
//void Mesh::Bind( ID3D12GraphicsCommandList* cmdList ) const
//{
//	cmdList->IASetVertexBuffers( 0, 1, &vertexBuffer.GetView() );
//	cmdList->IASetIndexBuffer( &indexBuffer.GetView() );
//	cmdList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
//}
//
//void Mesh::Cleanup()
//{
//	// No need to manually reset resources; handled by VertexBuffer/IndexBuffer destructors.
//	vertices.clear();
//	indices.clear();
//	indexCount = 0;
//}