#include <d3d12.h>
#include <Windows.h>
#include <wrl\client.h>
#include "DescriptorHeapManager.h"
#include "GraphPass.h"
#include "ImGuiLayer.h"
#include "SystemManager.h"
#include "Types.h"
#include "UIPass.h"

void UIPass::Execute( SystemManager& systemManager, const AssetID& sceneID )
{
	auto* renderer = systemManager.GetRenderer();
	auto* srvHeapManager = systemManager.GetSrvHeapManager();
	auto* uiLayer = systemManager.GetUILayer();

	uint frameIndex = renderer->GetCurrentFrameIndex();
	auto* allocator = commandAllocators[frameIndex].Get();
	auto* commandList = commandLists[frameIndex].Get();

	allocator->Reset();
	commandList->Reset( allocator, nullptr );

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderer->GetCurrentRtvHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = renderer->GetCurrentDsvHandle();
	commandList->OMSetRenderTargets( 1, &rtvHandle, FALSE, &dsvHandle );

	ID3D12DescriptorHeap* heaps[] = { srvHeapManager->GetHeap() };
	commandList->SetDescriptorHeaps( 1, heaps );

	uiLayer->BeginFrame();
	uiLayer->RenderUI();
	uiLayer->EndFrame( commandList );

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = renderer->GetCurrentBackBuffer();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// Transition the back buffer to PRESENT
	commandList->ResourceBarrier( 1, &barrier );

	commandList->Close();
}