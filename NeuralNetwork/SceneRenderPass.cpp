#include "SceneRenderPass.h"
#include "Renderer.h"
#include "Camera.h"
#include "Light.h"
#include "Actor.h"
#include "Mesh.h"
#include <DirectXMath.h>
#include "d3dx12.h"
#include <d3d12.h>
#include <fstream>
#include <unordered_map>

SceneRenderPass::SceneRenderPass(Scene* scene, Renderer& renderer)
    : RenderPass("ScenePass"), scene(scene)
{
    InitCommandResources(renderer);
    InitConstantBuffer(renderer);
    InitActorMeshes(renderer);
    InitActorWorldMatrixBuffer(renderer);

    // Load shaders
    vsBlob = renderer.LoadShaderBlob(L"Shaders\\ActorVS.cso");
    psBlob = renderer.LoadShaderBlob(L"Shaders\\ActorPS.cso");

    CreateRootSignature(renderer);
    CreateSRVs(renderer);
    CreatePipelineState(renderer);
}

void SceneRenderPass::Execute(Renderer& renderer) {

    commandAllocators[0]->Reset();
    commandLists[0]->Reset(commandAllocators[0].Get(), nullptr);

    D3D12_VIEWPORT viewport = { 0, 0, static_cast<float>(renderer.GetWidth()), static_cast<float>(renderer.GetHeight()), 0.0f, 1.0f };
    D3D12_RECT scissorRect = { 0, 0, (LONG)renderer.GetWidth(), (LONG)renderer.GetHeight() };
    commandLists[0]->RSSetViewports(1, &viewport);
    commandLists[0]->RSSetScissorRects(1, &scissorRect);


    auto rtvHandle = renderer.GetCurrentRtvHandle();
    auto dsvHandle = renderer.GetCurrentDsvHandle();

    commandLists[0]->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

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

    // Set root signature and pipeline state
    commandLists[0]->SetGraphicsRootSignature(rootSignature.Get());
    commandLists[0]->SetPipelineState(pipelineState.Get());

    // Set descriptor heap(s)
    ID3D12DescriptorHeap* heaps[] = { renderer.GetSrvHeap() };
    commandLists[0]->SetDescriptorHeaps(_countof(heaps), heaps);

    UINT actorWorldMatrixSrvIndex = 1; // Use the correct index for your SRV
    UINT descriptorSize = renderer.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuSrvHandle(
        renderer.GetSrvHeap()->GetGPUDescriptorHandleForHeapStart(),
        actorWorldMatrixSrvIndex,
        descriptorSize
    );

    // Set root parameters
    commandLists[0]->SetGraphicsRootConstantBufferView(0, sceneCB->GetGPUVirtualAddress());
    commandLists[0]->SetGraphicsRootDescriptorTable(1, gpuSrvHandle);

    // For each unique mesh, draw all actors that use it
    const auto& actors = scene->GetActors();
    if (actors.empty())
        return;

    // Group actors by mesh (optional, for efficiency)
    std::unordered_map<Mesh*, std::vector<size_t>> meshToActorIndices;
    for (size_t i = 0; i < actors.size(); ++i) {
        auto mesh = actors[i]->GetMesh().get();
        if (mesh)
            meshToActorIndices[mesh].push_back(i);
    }

    for (const auto& [mesh, actorIndices] : meshToActorIndices) {
        // Bind mesh vertex and index buffers
        mesh->Bind(commandLists[0].Get());

        // Draw each actor as an instance
        // If all actors for this mesh are contiguous, you can use DrawIndexedInstanced with instanceCount = actorIndices.size()
        // Otherwise, you may need to draw each instance separately or use a remapping buffer

        // For simplicity, draw each actor as a separate instance (can be optimized)
        for (size_t instance = 0; instance < actorIndices.size(); ++instance) {
            // Set the instance ID as the start instance location
            commandLists[0]->DrawIndexedInstanced(
                mesh->GetIndexCount(), // IndexCountPerInstance
                1,                     // InstanceCount
                0,                     // StartIndexLocation
                0,                     // BaseVertexLocation
                static_cast<UINT>(actorIndices[instance]) // StartInstanceLocation (matches world matrix index)
            );
        }
    }
}

void SceneRenderPass::InitCommandResources(Renderer& renderer) {
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    ID3D12Device* device = renderer.GetDevice();

    HRESULT hr = device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&commandAllocator)
    );

    if (SUCCEEDED(hr)) {
        hr = device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            commandAllocator.Get(),
            nullptr,
            IID_PPV_ARGS(&commandList)
        );

        #if defined(_DEBUG)
            commandList->SetName(L"Scene Command List");
            commandAllocator->SetName(L"Scene Command Allocator");
        #endif

        if (SUCCEEDED(hr)) {
            commandList->Close();
            AddCommandList(commandList, commandAllocator);
        }
    }
}

void SceneRenderPass::InitConstantBuffer(Renderer& renderer) {
    ID3D12Device* device = renderer.GetDevice();

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

    #if defined(_DEBUG)
        sceneCB->SetName(L"Scene Constant Buffer");
    #endif

    cbGpuAddress = sceneCB->GetGPUVirtualAddress();
}

void SceneRenderPass::InitActorMeshes(Renderer& renderer) {
    commandAllocators[0]->Reset();
    commandLists[0]->Reset(commandAllocators[0].Get(), nullptr);

    for (const auto& actor : scene->GetActors()) {
        auto mesh = actor->GetMesh();
        if (mesh) {
            mesh->CreateGPUResources(renderer, commandLists[0].Get());
        }
    }
    commandLists[0]->Close();

    ID3D12CommandList* cmdLists[] = { commandLists[0].Get() };
    renderer.GetCommandQueue()->ExecuteCommandLists(1, cmdLists);
    renderer.WaitForGpu();
}

void SceneRenderPass::InitActorWorldMatrixBuffer(Renderer& renderer) {
    ID3D12Device* device = renderer.GetDevice();
    const auto& actors = scene->GetActors();
    size_t actorCount = actors.size();
    actorWorldMatricesCPU.resize(actorCount);

    // Fill CPU-side buffer with world matrices
    for (size_t i = 0; i < actorCount; ++i) {
        const auto& actor = actors[i];
        DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(
            actor->GetScale().x, actor->GetScale().y, actor->GetScale().z);
        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion(
            DirectX::XMLoadFloat4(&actor->GetRotation()));
        DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(
            actor->GetPosition().x, actor->GetPosition().y, actor->GetPosition().z);
        DirectX::XMMATRIX world = scale * rotation * translation;
        DirectX::XMStoreFloat4x4(&actorWorldMatricesCPU[i].worldMatrix, DirectX::XMMatrixTranspose(world));
    }

    // Create upload heap for the structured buffer
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC resDesc = {};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = (sizeof(ActorWorldMatrixSB) * actorCount + 255) & ~255; // 256-byte aligned
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
        IID_PPV_ARGS(&actorWorldMatrixSB)
    );

    #if defined(_DEBUG)
        actorWorldMatrixSB->SetName(L"Actor World Matrices");
    #endif

    // Upload initial data
    void* mapped = nullptr;
    actorWorldMatrixSB->Map(0, nullptr, &mapped);
    memcpy(mapped, actorWorldMatricesCPU.data(), sizeof(ActorWorldMatrixSB) * actorCount);
    actorWorldMatrixSB->Unmap(0, nullptr);
}

void SceneRenderPass::CreateRootSignature(Renderer& renderer) {
    CD3DX12_DESCRIPTOR_RANGE1 ranges[1]{};
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0: ActorWorldMatrixSB

    CD3DX12_ROOT_PARAMETER1 rootParameters[2]{};
    // b0: SceneCB
    rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
    // t0: ActorWorldMatrixSB
    rootParameters[1].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    rootSignatureDesc.Desc_1_1.NumParameters = _countof(rootParameters);
    rootSignatureDesc.Desc_1_1.pParameters = rootParameters;
    rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
    rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;
    rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature, error;
    D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
    renderer.GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    #if defined(_DEBUG)
	    rootSignature->SetName(L"Scene Root Signature");
    #endif
}

void SceneRenderPass::CreateSRVs(Renderer& renderer) {
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = static_cast<UINT>(actorWorldMatricesCPU.size());
    srvDesc.Buffer.StructureByteStride = sizeof(ActorWorldMatrixSB);
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    UINT actorWorldMatrixSrvIndex = 1;
    UINT descriptorSize = renderer.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(
        renderer.GetSrvHeap()->GetCPUDescriptorHandleForHeapStart(),
        actorWorldMatrixSrvIndex,
        descriptorSize
    );
    renderer.GetDevice()->CreateShaderResourceView(actorWorldMatrixSB.Get(), &srvDesc, srvHandle);
}

void SceneRenderPass::CreatePipelineState(Renderer& renderer) {
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
    psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };

    // Rasterizer state
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    // Blend state
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    // Depth/stencil state
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT; // Or your depth format

    // Primitive topology
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // Render target formats
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // Match your swap chain format

    // Sample settings
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.SampleMask = UINT_MAX;

    // Create the pipeline state
    renderer.GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

    #if defined(_DEBUG)
	    pipelineState->SetName(L"Scene Pipeline State");
    #endif
}

void SceneRenderPass::Shutdown() {
    if (sceneCB) {
        sceneCB->Release();
        sceneCB = nullptr;
    }
    if (actorWorldMatrixSB) {
        actorWorldMatrixSB->Release();
        actorWorldMatrixSB = nullptr;
    }
    commandLists.clear();
    commandAllocators.clear();
}