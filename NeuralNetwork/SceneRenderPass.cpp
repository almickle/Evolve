#include "SceneRenderPass.h"
//#include "Renderer.h"
//#include "Camera.h"
//#include "Light.h"
//#include "Actor.h"
//#include "Mesh.h"
//#include <DirectXMath.h>
//#include "d3dx12.h"
//#include <d3d12.h>
//#include <fstream>
//#include <unordered_map>
//
//SceneRenderPass::SceneRenderPass(Scene* scene, Renderer& renderer)
//    : GraphPass("ScenePass"), scene(scene)
//{
//    InitCommandResources(renderer);
//    InitConstantBuffer(renderer);
//    CreateRootSignature(renderer);
//}
//
//void SceneRenderPass::Execute(Renderer& renderer) {
//    Begin(renderer);
//    Update(renderer);
//    Render(renderer);
//}
//
//void SceneRenderPass::Begin(Renderer& renderer) {
//    commandAllocators[0]->Reset();
//    commandLists[0]->Reset(commandAllocators[0].Get(), nullptr);
//
//    auto rtvHandle = renderer.GetCurrentRtvHandle();
//    auto dsvHandle = renderer.GetCurrentDsvHandle();
//    commandLists[0]->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
//
//    D3D12_VIEWPORT viewport = { 0, 0, static_cast<float>(renderer.GetWidth()), static_cast<float>(renderer.GetHeight()), 0.0f, 1.0f };
//    D3D12_RECT scissorRect = { 0, 0, (LONG)renderer.GetWidth(), (LONG)renderer.GetHeight() };
//    commandLists[0]->RSSetViewports(1, &viewport);
//    commandLists[0]->RSSetScissorRects(1, &scissorRect);
//
//    // Set descriptor heap(s)
//    ID3D12DescriptorHeap* heaps[] = { renderer.GetSrvHeapManager()->GetHeap() };
//    commandLists[0]->SetDescriptorHeaps(_countof(heaps), heaps);
//
//    // Set root signature
//    commandLists[0]->SetGraphicsRootSignature(rootSignature.Get());
//
//    // Set root parameters: scene constant buffer and instance buffer SRV
//    commandLists[0]->SetGraphicsRootConstantBufferView(0, sceneCB.GetGpuAddress());
//    commandLists[0]->SetGraphicsRootDescriptorTable(1, scene->GetInstanceBuffer().GetSrvGpuHandle());
//}
//
//void SceneRenderPass::Update(Renderer& renderer) {
//    UpdateConstantBuffer(renderer);
//}
//
//void SceneRenderPass::Render(Renderer& renderer) {
//    const auto& models = scene->GetInstanceManager().GetModels();
//
//    for (const auto& [modelId, modelPtr] : models) {
//        // Get all instance data for this model
//        const std::vector<InstanceData> instances = scene->GetInstanceManager().GetAllInstanceData(modelId);
//        if (instances.empty()) continue;
//
//        // For each mesh in the model
//        const auto& meshes = modelPtr->GetMeshes();
//        const auto& materials = modelPtr->GetMaterials();
//
//        for (size_t meshIdx = 0; meshIdx < meshes.size(); ++meshIdx) {
//            const auto& mesh = meshes[meshIdx];
//            const auto& material = (meshIdx < materials.size()) ? materials[meshIdx] : nullptr;
//
//            // Bind material (if any)
//            if (material) {
//                material->Bind(commandLists[0].Get());
//            }
//
//            // Bind mesh
//            mesh->Bind(commandLists[0].Get());
//
//            // Draw all instances of this mesh
//            commandLists[0]->DrawIndexedInstanced(
//                mesh->GetIndexCount(),
//                static_cast<uint>(instances.size()),
//                0, 0, 0
//            );
//        }
//    }
//
//    commandLists[0]->Close();
//}
//
//void SceneRenderPass::UpdateConstantBuffer(Renderer& renderer) {
//    // Assume first camera is active
//    const auto& cameras = scene->GetCameras();
//    if (!cameras.empty()) {
//        const auto& cam = *cameras[0];
//        DirectX::XMStoreFloat4x4(&cbData.camera.view, DirectX::XMMatrixTranspose(cam.GetViewMatrix()));
//        DirectX::XMStoreFloat4x4(&cbData.camera.proj, DirectX::XMMatrixTranspose(cam.GetProjMatrix()));
//        cbData.camera.position = cam.GetPosition();
//    }
//    // Fill light data
//    const auto& lights = scene->GetLights();
//    cbData.numLights = static_cast<int>(lights.size());
//    if (!lights.empty()) {
//        for (int i = 0; i < cbData.numLights; ++i) {
//            const auto& l = *lights[i];
//            cbData.lights[i].direction = l.GetDirection();
//            cbData.lights[i].color = l.GetColor();
//            cbData.lights[i].intensity = l.GetIntensity();
//            cbData.lights[i].type = static_cast<int>(l.GetType());
//            cbData.lights[i].position = l.GetPosition();
//        }
//    }
//
//    // Upload to GPU using ConstantBuffer
//    sceneCB.Update(&cbData, sizeof(SceneCB));
//}
//
//void SceneRenderPass::InitCommandResources(Renderer& renderer) {
//    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
//    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
//    ID3D12Device* device = renderer.GetDevice();
//
//    HRESULT hr = device->CreateCommandAllocator(
//        D3D12_COMMAND_LIST_TYPE_DIRECT,
//        IID_PPV_ARGS(&commandAllocator)
//    );
//
//    if (SUCCEEDED(hr)) {
//        hr = device->CreateCommandList(
//            0,
//            D3D12_COMMAND_LIST_TYPE_DIRECT,
//            commandAllocator.Get(),
//            nullptr,
//            IID_PPV_ARGS(&commandList)
//        );
//
//        #if defined(_DEBUG)
//            commandList->SetName(L"Scene Command List");
//            commandAllocator->SetName(L"Scene Command Allocator");
//        #endif
//
//        if (SUCCEEDED(hr)) {
//            commandList->Close();
//            AddCommandList(commandList, commandAllocator);
//        }
//    }
//}
//
//void SceneRenderPass::InitConstantBuffer(Renderer& renderer) {
//    sceneCB.CreateResource(renderer, sizeof(SceneCB));
//}
//
//void SceneRenderPass::CreateRootSignature(Renderer& renderer) {
//    CD3DX12_DESCRIPTOR_RANGE1 ranges[1]{};
//    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0
//
//    CD3DX12_ROOT_PARAMETER1 rootParameters[2]{};
//    rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL); // b0
//    rootParameters[1].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX); // t0
//
//    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
//    rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
//    rootSignatureDesc.Desc_1_1.NumParameters = _countof(rootParameters);
//    rootSignatureDesc.Desc_1_1.pParameters = rootParameters;
//    rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
//    rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;
//    rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//    Microsoft::WRL::ComPtr<ID3DBlob> signature, error;
//    D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
//    renderer.GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
//
//    #if defined(_DEBUG)
//        rootSignature->SetName(L"Scene Root Signature");
//    #endif
//}
//
//void SceneRenderPass::Shutdown() {
//    commandLists.clear();
//    commandAllocators.clear();
//}