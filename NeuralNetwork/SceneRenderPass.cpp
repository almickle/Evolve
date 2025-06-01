#include "SceneRenderPass.h"
#include "Renderer.h"
#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include <DirectXMath.h>
#include "d3dx12.h"
#include <d3d12.h>

SceneRenderPass::SceneRenderPass(Scene* scene, Renderer& renderer)
    : RenderPass("ScenePass"), scene(scene)
{
    auto device = renderer.GetDevice();

    // Create upload heap for constant buffer
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC resDesc = {};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = (sizeof(SceneCB) + 255) & ~255; // CB size must be 256-byte aligned
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.SampleDesc.Count = 1;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&sceneCB)
    );

    cbGpuAddress = sceneCB->GetGPUVirtualAddress();

    for(const auto& mesh : scene->GetMeshes()) {
        mesh->CreateGPUResources(renderer);
	}
}

void SceneRenderPass::Execute(Renderer& renderer) {
    // Assume first camera is active
    const auto& cameras = scene->GetCameras();
    if (!cameras.empty()) {
        const auto& cam = *cameras[0];
        DirectX::XMStoreFloat4x4(&cbData.camera.view, DirectX::XMMatrixTranspose(cam.GetViewMatrix()));
        DirectX::XMStoreFloat4x4(&cbData.camera.proj, DirectX::XMMatrixTranspose(cam.GetProjMatrix()));
        cbData.camera.position = cam.GetPosition();
    }

    // Fill light data
    const auto& lights = scene->GetLights();
    cbData.numLights = static_cast<int>(lights.size());
    if (!lights.empty()) {
        for (int i = 0; i < cbData.numLights; ++i) {
            const auto& l = *lights[i];
            cbData.lights[i].direction = l.GetDirection();
            cbData.lights[i].color = l.GetColor();
            cbData.lights[i].intensity = l.GetIntensity();
            cbData.lights[i].type = static_cast<int>(l.GetType());
            cbData.lights[i].position = l.GetPosition();
        }
    }

    // Upload to GPU
    void* mapped = nullptr;
    sceneCB->Map(0, nullptr, &mapped);
    memcpy(mapped, &cbData, sizeof(SceneCB));
    sceneCB->Unmap(0, nullptr);

    // ... Bind the constant buffer to the pipeline as needed ...
}

void SceneRenderPass::Shutdown() {
    if (sceneCB) {
        sceneCB->Release();
        sceneCB = nullptr;
    }
    for (const auto& mesh : scene->GetMeshes()) {
        mesh->Cleanup();
    }
}