#pragma once
#include "RenderPass.h"
#include "Scene.h"
#include <wrl.h>
#include <d3d12.h>
#include "SceneCB.h"

class SceneRenderPass : public RenderPass {
public:
    SceneRenderPass(Scene* scene, Renderer& renderer);
    void Execute(Renderer& renderer) override;
    void Shutdown() override;
private:
    Scene* scene;
    Microsoft::WRL::ComPtr<ID3D12Resource> sceneCB;
    SceneCB cbData;
    D3D12_GPU_VIRTUAL_ADDRESS cbGpuAddress = 0;
};