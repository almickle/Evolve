#include <d3d12.h>
#include <Windows.h>
#include "AssetManager.h"
#include "ConstantBuffer.h"
#include "DescriptorHeapManager.h"
#include "GpuResourceManager.h"
#include "Model.h"
#include "Renderer.h"
#include "Scene.h"
#include "ScenePass.h"
#include "SystemManager.h"
#include "Types.h"

void ScenePass::Execute( SystemManager& systemManager, const AssetID& sceneID )
{
	auto* renderer = systemManager.GetRenderer();
	auto* assetManager = systemManager.GetAssetManager();
	auto* resourceManager = systemManager.GetResourceManager();
	auto* srvHeapManager = systemManager.GetSrvHeapManager();

	renderer->UpdateCurrentFrameIndex();
	uint frameIndex = renderer->GetCurrentFrameIndex();

	auto allocator = commandAllocators[frameIndex].Get();
	auto commandList = commandLists[frameIndex].Get();

	allocator->Reset();
	commandList->Reset( allocator, nullptr );

	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = renderer->GetCurrentBackBuffer();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		// Transition the back buffer to RENDER_TARGET
		commandList->ResourceBarrier( 1, &barrier );
	}

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderer->GetCurrentRtvHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = renderer->GetCurrentDsvHandle();
	commandList->OMSetRenderTargets( 1, &rtvHandle, FALSE, &dsvHandle );

	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView( rtvHandle, clearColor, 0, nullptr );
	commandList->ClearDepthStencilView( dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr );

	renderer->SetViewport( commandList );
	renderer->SetScissorRect( commandList );

	ID3D12DescriptorHeap* heaps[] = { srvHeapManager->GetHeap() };
	commandList->SetDescriptorHeaps( 1, heaps );
	commandList->IASetPrimitiveTopology( renderer->GetTopology() );
	commandList->SetGraphicsRootSignature( renderer->GetRootSignature() );
	commandList->SetGraphicsRootDescriptorTable( 5, srvHeapManager->GetHeap()->GetGPUDescriptorHandleForHeapStart() );

	auto* scene = static_cast<Scene*>(assetManager->GetAsset( sceneID ));
	auto* sManager = scene->GetStaticInstanceManager();
	auto* dManager = scene->GetDynamicInstanceManager();

	auto* cb = static_cast<ConstantBuffer*>(resourceManager->GetResource( scene->GetSceneBuffer() ));
	renderer->BindSceneConstantBuffer( commandList, cb->GetResource()->GetGPUVirtualAddress() );

	for( auto& uniqueModel : sManager->GetUniqueAssets() )
	{
		uint instanceCount = sManager->GetInstanceCount( uniqueModel );
		uint heapOffset = sManager->GetHeapOffset( uniqueModel );

		auto* model = static_cast<Model*>(assetManager->GetAsset( uniqueModel ));
		auto psCbAddresses = model->GetMaterialConstantBufferAddresses( *assetManager, *resourceManager );

		auto& psoKeys = model->GetPsoKeys();
		auto vbViews = model->GetVertexBufferViews( *assetManager, *resourceManager );
		auto ibViews = model->GetIndexBufferViews( *assetManager, *resourceManager );

		renderer->RenderActorInstances( commandList, psoKeys, vbViews, ibViews, psCbAddresses, instanceCount, heapOffset, true );
	}

	for( auto& uniqueModel : dManager->GetUniqueAssets() )
	{
		uint instanceCount = dManager->GetInstanceCount( uniqueModel );
		uint heapOffset = dManager->GetHeapOffset( uniqueModel );

		auto* model = static_cast<Model*>(assetManager->GetAsset( uniqueModel ));
		auto psCbAddresses = model->GetMaterialConstantBufferAddresses( *assetManager, *resourceManager );

		auto& psoKeys = model->GetPsoKeys();
		auto vbViews = model->GetVertexBufferViews( *assetManager, *resourceManager );
		auto ibViews = model->GetIndexBufferViews( *assetManager, *resourceManager );

		renderer->RenderActorInstances( commandList, psoKeys, vbViews, ibViews, psCbAddresses, instanceCount, heapOffset, true );
	}

	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = systemManager.GetRenderer()->GetCurrentBackBuffer();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		// Transition the back buffer to PRESENT
		commandList->ResourceBarrier( 1, &barrier );
	}

	commandList->Close();
}