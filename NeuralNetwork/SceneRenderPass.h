//#pragma once
//#include "GraphPass.h"
//#include "Scene.h"
//#include <wrl/client.h>
//#include <d3d12.h>
//#include "SceneCB.h"
//#include "StructuredBuffer.h"
//#include "ConstantBuffer.h"
//#include <vector>
//
//class SceneRenderPass : public GraphPass {
//public:
//    SceneRenderPass(Scene* scene, Renderer& renderer);
//public:
//    void Execute(Renderer& renderer) override;
//    void Shutdown() override;
//private:
//    void Begin(Renderer& renderer);
//    void Update(Renderer& renderer);
//    void Render(Renderer& renderer);
//private:
//    void InitCommandResources(Renderer& renderer);
//    void InitConstantBuffer(Renderer& renderer);
//    void CreateRootSignature(Renderer& renderer);
//public:
//    void UpdateConstantBuffer(Renderer& renderer);
//
//    Scene* scene;
//
//    // Constant buffer for scene data
//    ConstantBuffer sceneCB;
//    SceneCB cbData = {};
//
//    // Root signature
//    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
//
//    // Input layout for position, normal, texcoord, tangent, instanceID
//    static constexpr D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
//        { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
//        { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
//        { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0, 24,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
//        { "TANGENT",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
//        { "INSTANCEID", 0, DXGI_FORMAT_R32_uint,        1, 0,   D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
//    };
//};