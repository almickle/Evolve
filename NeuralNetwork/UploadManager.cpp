#include <atomic>
#include <combaseapi.h>
#include <cstdint>
#include <d3d12.h>
#include <mutex>
#include <queue>
#include <synchapi.h>
#include <utility>
#include <Windows.h>
#include "Renderer.h"
#include "ThreadManager.h"
#include "Types.h"
#include "UploadManager.h"

void UploadManager::Init()
{
	// Create upload allocator and command list
	auto device = renderer->GetDevice();
	device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &uploadAllocator ) );
	device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, uploadAllocator.Get(), nullptr, IID_PPV_ARGS( &uploadCmdList ) );
	uploadCmdList->Close(); // Start closed

	// Create upload fence
	device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &uploadFence ) );
	uploadFenceValue = 1;
	uploadFenceEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );

	for( uint i = 0; i < Renderer::BackBufferCount; i++ )
	{
		// Create allocators and command lists for each frame
		auto device = renderer->GetDevice();
		device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &frameAllocators[i] ) );
		device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameAllocators[i].Get(), nullptr, IID_PPV_ARGS( &frameCmdLists[i] ) );
		frameCmdLists[i]->Close(); // Start closed
	}
}

void UploadManager::Enqueue( UploadRequest req )
{
	std::lock_guard<std::mutex> lock( queueMutex );
	uploadQueue.push( std::move( req ) );
}

void UploadManager::Flush()
{
	// Only one batch at a time
	if( batchInProgress.exchange( true ) ) return;

	std::queue<UploadRequest> localQueue;
	{
		std::lock_guard<std::mutex> lock( queueMutex );
		std::swap( localQueue, uploadQueue );
	}

	if( localQueue.empty() ) {
		batchInProgress = false;
		return;
	}

	// Reset allocator and command list once
	uploadAllocator->Reset();
	uploadCmdList->Reset( uploadAllocator.Get(), nullptr );

	// Record all upload commands
	while( !localQueue.empty() ) {
		localQueue.front().recordFunc( uploadCmdList.Get() );
		localQueue.pop();
	}

	uploadCmdList->Close();
	ID3D12CommandList* lists[] = { uploadCmdList.Get() };
	renderer->GetCommandQueue()->ExecuteCommandLists( 1, lists );

	uploadFenceValue++;
	renderer->GetCommandQueue()->Signal( uploadFence.Get(), uploadFenceValue );

	// Optionally, wait for completion and call all callbacks
	// (You can track callbacks in a vector if needed)

	batchInProgress = false;

	WaitForInitialUpload();
}

void UploadManager::FlushCurrentFrame()
{
	// Only one batch at a time
	if( batchInProgress.exchange( true ) ) return;

	auto frameIndex = renderer->GetCurrentFrameIndex();

	threadManager->Launch( [this, frameIndex] {
		std::queue<UploadRequest> localQueue;
		{
			std::lock_guard<std::mutex> lock( queueMutex );
			std::swap( localQueue, uploadQueue );
		}

		if( localQueue.empty() ) {
			batchInProgress = false;
			return;
		}

		// Reset allocator and command list once
		frameAllocators[frameIndex]->Reset();
		frameCmdLists[frameIndex]->Reset( frameAllocators[frameIndex].Get(), nullptr );

		// Record all upload commands
		while( !localQueue.empty() ) {
			localQueue.front().recordFunc( frameCmdLists[frameIndex].Get() );
			localQueue.pop();
		}

		// Optionally, wait for completion and call all callbacks
		// (You can track callbacks in a vector if needed)

		batchInProgress = false;
						   } );
}

void UploadManager::WaitForInitialUpload()
{
	const uint64_t fenceToWait = ++uploadFenceValue;
	renderer->GetCommandQueue()->Signal( uploadFence.Get(), fenceToWait );
	if( uploadFence->GetCompletedValue() < fenceToWait ) {
		uploadFence->SetEventOnCompletion( fenceToWait, uploadFenceEvent );
		WaitForSingleObject( uploadFenceEvent, INFINITE );
	}
}