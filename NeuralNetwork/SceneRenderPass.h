#pragma once
#include "RenderPass.h"
#include "Scene.h"
#include <wrl/client.h>
#include <d3d12.h>
#include "SceneCB.h"
#include <vector>

class SceneRenderPass : public RenderPass {
public:
    SceneRenderPass(Scene* scene, Renderer& renderer);
    void Execute(Renderer& renderer) override;
    void Shutdown() override;

private:
    void InitCommandResources(Renderer& renderer);
    void InitConstantBuffer(Renderer& renderer);
    void InitActorMeshes(Renderer& renderer);
    void InitActorWorldMatrixBuffer(Renderer& renderer);

    // New pipeline setup functions
    void CreateRootSignature(Renderer& renderer);
    void CreateSRVs(Renderer& renderer);
    void CreatePipelineState(Renderer& renderer);

    Scene* scene;

    // Constant buffer for scene data
    Microsoft::WRL::ComPtr<ID3D12Resource> sceneCB;
    D3D12_GPU_VIRTUAL_ADDRESS cbGpuAddress = 0;
    SceneCB cbData = {};

    // Structured buffer for actor world matrices
    Microsoft::WRL::ComPtr<ID3D12Resource> actorWorldMatrixSB;
    std::vector<ActorWorldMatrixSB> actorWorldMatricesCPU;

    // Root signature and pipeline state
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

    // Shader blobs and input layout (add as needed)
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;

    // Input layout for position, normal, texcoord
    static constexpr D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
        { "INSTANCEID", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
    };
};