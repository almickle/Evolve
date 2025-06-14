#include <combaseapi.h>
#include <d3d12.h>
#include <Windows.h>
#include "BeginFramePass.h"
#include "DescriptorHeapManager.h"
#include "Renderer.h"
#include "SystemManager.h"
#include "Types.h"

void BeginFramePass::Init( SystemManager& systemManager )
{
	// Allocate per-frame command allocators and command lists
	uint frames = Renderer::BackBufferCount;

	for( uint i = 0; i < frames; ++i ) {
		systemManager.GetRenderer()->GetDevice()->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS( &commandAllocators[i] )
		);
		systemManager.GetRenderer()->GetDevice()->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocators[i].Get(),
			nullptr,
			IID_PPV_ARGS( &commandLists[i] )
		);
		commandLists[i]->Close();
	}
}

void BeginFramePass::Execute( SystemManager& systemManager )
{
	auto* renderer = systemManager.GetRenderer();
	auto* srvHeapManager = systemManager.GetSrvHeapManager();

	renderer->UpdateCurrentFrameIndex();
	uint frameIndex = renderer->GetCurrentFrameIndex();

	auto allocator = commandAllocators[frameIndex].Get();
	auto commandList = commandLists[frameIndex].Get();

	allocator->Reset();
	commandList->Reset( allocator, nullptr );

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = renderer->GetCurrentBackBuffer();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier( 1, &barrier );

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderer->GetCurrentRtvHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = renderer->GetCurrentDsvHandle();
	commandList->OMSetRenderTargets( 1, &rtvHandle, FALSE, &dsvHandle );

	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView( rtvHandle, clearColor, 0, nullptr );
	commandList->ClearDepthStencilView( dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr );

	ID3D12DescriptorHeap* heaps[] = { systemManager.GetSrvHeapManager()->GetHeap() };
	commandList->SetDescriptorHeaps( 1, heaps );
	commandList->IASetPrimitiveTopology( renderer->GetTopology() );
	commandList->SetGraphicsRootSignature( renderer->GetRootSignature() );
	commandList->SetGraphicsRootDescriptorTable( 5, srvHeapManager->GetHeap()->GetGPUDescriptorHandleForHeapStart() );

	commandList->Close();
}
