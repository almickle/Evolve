#include <combaseapi.h>
#include <d3d12.h>
#include <future>
#include <mutex>
#include <synchapi.h>
#include <thread>
#include <utility>
#include <vector>
#include <Windows.h>
#include "Renderer.h"
#include "UploadManager.h"

UploadManager::UploadManager( Renderer& renderer )
	: running( true )
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

	// Start the upload thread
	uploadThread = std::thread( [this, &renderer]() {
		this->StartUploadThread( renderer );
								} );
}

UploadManager::~UploadManager()
{
	Shutdown();
}

void UploadManager::StartUploadThread( Renderer& renderer )
{
	while( running ) {
		std::unique_lock lock( mutex );
		cv.wait( lock, [&] { return !queue.empty() || !running; } );

		// Batch all requests currently in the queue
		std::vector<UploadRequest> batch;
		while( !queue.empty() ) {
			batch.push_back( std::move( queue.front() ) );
			queue.pop();
		}
		lock.unlock();

		if( !batch.empty() ) {
			uploadAllocator->Reset();
			uploadCmdList->Reset( uploadAllocator.Get(), nullptr );

			// Record all uploads in one command list
			for( auto& req : batch ) {
				req.recordFunc( uploadCmdList.Get() );
			}
			uploadCmdList->Close();

			ID3D12CommandList* lists[] = { uploadCmdList.Get() };
			uploadCommandQueue->ExecuteCommandLists( 1, lists );
			uploadCommandQueue->Signal( uploadFence.Get(), uploadFenceValue );

			// Call all onComplete callbacks
			for( auto& req : batch ) {
				if( req.onComplete ) req.onComplete();
			}
		}
	}
}

void UploadManager::Enqueue( UploadRequest req )
{
	{
		std::lock_guard<std::mutex> lock( mutex );
		queue.push( std::move( req ) );
	}
	cv.notify_one();
}

void UploadManager::Shutdown()
{
	running = false;
	cv.notify_one();
	if( uploadThread.joinable() ) {
		uploadThread.join();
	}
}

void UploadManager::Flush()
{
	// Enqueue a no-op upload that captures the current fence value
	std::promise<void> promise;
	auto future = promise.get_future();

	Enqueue( {
		// recordFunc: no-op
		[]( ID3D12GraphicsCommandList* ) {},
		// onComplete: signal fence and set promise
		[this, &promise]() {
			uploadCommandQueue->Signal( uploadFence.Get(), uploadFenceValue );
			if( uploadFence->GetCompletedValue() < uploadFenceValue ) {
				uploadFence->SetEventOnCompletion( uploadFenceValue, uploadFenceEvent );
				WaitForSingleObject( uploadFenceEvent, INFINITE );
			}
			++uploadFenceValue;
			promise.set_value();
		}
			 } );

	// Wait for the future to complete
	future.wait();
}