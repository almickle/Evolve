#include "Mesh.h"
#include "Renderer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>
#include <wrl.h>
#include <d3d12.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

Mesh::Mesh(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices);

    if (!scene || !scene->HasMeshes()) {
        std::string msg = std::string(importer.GetErrorString());
        throw std::runtime_error("Failed to load mesh: " + path + msg);
    }

    const aiMesh* mesh = scene->mMeshes[0];

    vertices.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex v;
        v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        v.normal = mesh->HasNormals() ? DirectX::XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : DirectX::XMFLOAT3(0,0,0);
        if (mesh->HasTextureCoords(0))
            v.texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        else
            v.texcoord = { 0, 0 };
        vertices.push_back(v);
    }

    indices.reserve(mesh->mNumFaces * 3);
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }
    indexCount = static_cast<UINT>(indices.size());
}

void Mesh::CreateGPUResources(Renderer& renderer, ID3D12GraphicsCommandList* cmdList)
{
    ID3D12Device* device = renderer.GetDevice();

    // Vertex Buffer
    UINT vbSize = static_cast<UINT>(vertices.size() * sizeof(Vertex));

    CD3DX12_HEAP_PROPERTIES defaultHeap(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);
    HRESULT hr = device->CreateCommittedResource(
        &defaultHeap, D3D12_HEAP_FLAG_NONE, &vbDesc,
        D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&vertexBuffer));
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create vertex buffer resource. HRESULT: " + std::to_string(hr));
    }

    #if defined(_DEBUG)
        vertexBuffer->SetName(L"Vertex Buffer");
    #endif

    CD3DX12_RESOURCE_BARRIER copyBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        vertexBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
    cmdList->ResourceBarrier(1, &copyBarrier);

    CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
    device->CreateCommittedResource(
        &uploadHeap, D3D12_HEAP_FLAG_NONE, &vbDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vbUpload));

    #if defined(_DEBUG)
        vbUpload->SetName(L"Vertex Upload Buffer");
    #endif

    void* mapped = nullptr;
    vbUpload->Map(0, nullptr, &mapped);
    memcpy(mapped, vertices.data(), vbSize);
    vbUpload->Unmap(0, nullptr);

    cmdList->CopyBufferRegion(vertexBuffer.Get(), 0, vbUpload.Get(), 0, vbSize);

    CD3DX12_RESOURCE_BARRIER vbBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    cmdList->ResourceBarrier(1, &vbBarrier);

    vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vbView.SizeInBytes = vbSize;
    vbView.StrideInBytes = sizeof(Vertex);

    // Index Buffer
    UINT ibSize = static_cast<UINT>(indices.size() * sizeof(uint32_t));
    CD3DX12_RESOURCE_DESC ibDesc = CD3DX12_RESOURCE_DESC::Buffer(ibSize);

    device->CreateCommittedResource(
        &defaultHeap, D3D12_HEAP_FLAG_NONE, &ibDesc,
        D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&indexBuffer));

    #if defined(_DEBUG)
        vbUpload->SetName(L"Index Buffer");
    #endif

    CD3DX12_RESOURCE_BARRIER ibCpBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        indexBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
    cmdList->ResourceBarrier(1, &ibCpBarrier);

    device->CreateCommittedResource(
        &uploadHeap, D3D12_HEAP_FLAG_NONE, &ibDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&ibUpload));

    #if defined(_DEBUG)
        vbUpload->SetName(L"Index Upload Buffer");
    #endif

    ibUpload->Map(0, nullptr, &mapped);
    memcpy(mapped, indices.data(), ibSize);
    ibUpload->Unmap(0, nullptr);

    cmdList->CopyBufferRegion(indexBuffer.Get(), 0, ibUpload.Get(), 0, ibSize);

    CD3DX12_RESOURCE_BARRIER ibBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
    cmdList->ResourceBarrier(1, &ibBarrier);

    ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    ibView.SizeInBytes = ibSize;
    ibView.Format = DXGI_FORMAT_R32_UINT;

    // Note: Keep vbUpload and ibUpload alive until the GPU is done with the copy.
}

void Mesh::Bind(ID3D12GraphicsCommandList* cmdList) const {
    cmdList->IASetVertexBuffers(0, 1, &vbView);
    cmdList->IASetIndexBuffer(&ibView);
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::Cleanup() {
    vertexBuffer.Reset();
    indexBuffer.Reset();
    vertices.clear();
    indices.clear();
    indexCount = 0;
}