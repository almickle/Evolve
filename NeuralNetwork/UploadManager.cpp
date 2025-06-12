#include <atomic>
#include <combaseapi.h>
#include <d3d12.h>
#include <mutex>
#include <queue>
#include <synchapi.h>
#include <utility>
#include <Windows.h>
#include "Renderer.h"
#include "UploadManager.h"

UploadManager::UploadManager( Renderer& renderer )
	: threadManager( renderer.GetThreadManager() )
{
	// Create upload allocator and command list
	auto device = renderer.GetDevice();
	device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS( &uploadAllocator ) );
	device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_COPY, uploadAllocator.Get(), nullptr, IID_PPV_ARGS( &uploadCmdList ) );
	uploadCmdList->Close(); // Start closed

	// Create upload command queue
	D3D12_COMMAND_QUEUE_DESC copyQueueDesc = {};
	copyQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	copyQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	device->CreateCommandQueue( &copyQueueDesc, IID_PPV_ARGS( &uploadCommandQueue ) );

	// Create upload fence
	device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &uploadFence ) );
	uploadFenceValue = 1;
	uploadFenceEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );
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

	threadManager->Launch( [this] {
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
		uploadCommandQueue->ExecuteCommandLists( 1, lists );

		uploadFenceValue++;
		uploadCommandQueue->Signal( uploadFence.Get(), uploadFenceValue );

		// Optionally, wait for completion and call all callbacks
		// (You can track callbacks in a vector if needed)

		batchInProgress = false;
						   } );
}