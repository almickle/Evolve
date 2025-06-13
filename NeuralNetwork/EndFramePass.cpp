#include <combaseapi.h>
#include <d3d12.h>
#include "EndFramePass.h"
#include "Renderer.h"
#include "SystemManager.h"
#include "Types.h"

void EndFramePass::Init( SystemManager& systemManager )
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

void EndFramePass::Execute( SystemManager& systemManager )
{
	uint frameIndex = systemManager.GetRenderer()->GetCurrentFrameIndex();
	auto allocator = commandAllocators[frameIndex].Get();
	auto commandList = commandLists[frameIndex].Get();

	allocator->Reset();
	commandList->Reset( allocator, nullptr );

	// Transition the back buffer to PRESENT
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = systemManager.GetRenderer()->GetCurrentBackBuffer();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier( 1, &barrier );

	commandList->Close();
}