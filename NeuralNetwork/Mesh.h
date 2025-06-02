#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <DirectXMath.h>

class Renderer; // Forward declaration

class Mesh {
public:
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
    };

    Mesh(const std::string& path);

    void CreateGPUResources(Renderer& renderer, ID3D12GraphicsCommandList* cmdList);
    void Bind(ID3D12GraphicsCommandList* cmdList) const;

    const std::vector<Vertex>& GetVertices() const { return vertices; }
    const std::vector<uint32_t>& GetIndices() const { return indices; }
    const D3D12_VERTEX_BUFFER_VIEW& GetVBView() const { return vbView; }
    const D3D12_INDEX_BUFFER_VIEW& GetIBView() const { return ibView; }
    UINT GetIndexCount() const { return indexCount; }

	void Cleanup();

private:
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    UINT indexCount = 0;

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> vbUpload;
    Microsoft::WRL::ComPtr<ID3D12Resource> ibUpload;
    D3D12_VERTEX_BUFFER_VIEW vbView{};
    D3D12_INDEX_BUFFER_VIEW ibView{};
};