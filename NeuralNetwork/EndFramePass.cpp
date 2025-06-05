#include <combaseapi.h>
#include <d3d12.h>
#include <Windows.h>
#include "EndFramePass.h"
#include "Renderer.h"

void EndFramePass::Execute( Renderer& renderer )
{
	uint frameIndex = renderer.GetCurrentFrameIndex();
	auto allocator = commandAllocators[frameIndex].Get();
	auto commandList = commandLists[frameIndex].Get();

	allocator->Reset();
	commandList->Reset( allocator, nullptr );

	// Transition the back buffer to PRESENT
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = renderer.GetCurrentBackBuffer();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier( 1, &barrier );

	commandList->Close();
}


void EndFramePass::Init( Renderer& renderer )
{
	// Allocate per-frame command allocators and command lists
	uint frames = Renderer::BackBufferCount;

	for( uint i = 0; i < frames; ++i ) {
		renderer.GetDevice()->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS( &commandAllocators[i] )
		);
		renderer.GetDevice()->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocators[i].Get(),
			nullptr,
			IID_PPV_ARGS( &commandLists[i] )
		);
		commandLists[i]->Close();
	}
}
