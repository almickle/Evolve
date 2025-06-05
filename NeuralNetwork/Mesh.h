//#pragma once
//#include <cstdint>
//#include <d3d12.h>
//#include <DirectXMath.h>
//#include <string>
//#include <vector>
//#include <Windows.h>
//#include "IndexBuffer.h"
//#include "VertexBuffer.h"
//
//class Renderer; // Forward declaration
//
//struct Vertex {
//	DirectX::XMFLOAT3 position;
//	DirectX::XMFLOAT3 normal;
//	DirectX::XMFLOAT2 texcoord;
//	DirectX::XMFLOAT3 tangent; // Added tangent support
//};
//
//class Mesh {
//public:
//	Mesh( const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& name = "" );
//public:
//	void CreateGPUResources( Renderer& renderer );
//	void Bind( ID3D12GraphicsCommandList* cmdList ) const;
//	void Cleanup();
//public:
//	const std::vector<Vertex>& GetVertices() const { return vertices; }
//	const std::vector<uint32_t>& GetIndices() const { return indices; }
//	uint GetIndexCount() const { return indexCount; }
//private:
//	std::string name;
//	std::vector<Vertex> vertices;
//	std::vector<uint32_t> indices;
//	uint indexCount = 0;
//private:
//	VertexBuffer vertexBuffer;
//	IndexBuffer indexBuffer;
//};