#pragma once
#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <Windows.h>
#include <wrl/client.h>
#include "d3d12.h"
#include "Renderer.h"
#include "System.h"
#include "SystemManager.h"

class ThreadManager;

struct UploadRequest {
	std::function<void( ID3D12GraphicsCommandList* )> recordFunc;
	std::function<void()> onComplete; // Optional callback
};

using Microsoft::WRL::ComPtr;

class UploadManager : public System {
public:
	UploadManager( SystemManager& systemManager )
		: threadManager( systemManager.GetThreadManager() ),
		renderer( systemManager.GetRenderer() )
	{
	}
	~UploadManager() = default;
public:
	void Init();
	void Enqueue( UploadRequest req );
	void Flush();
	void FlushCurrentFrame();
private:
	void WaitForInitialUpload();
private:
	std::mutex queueMutex;
	std::queue<UploadRequest> uploadQueue;
	std::atomic<bool> batchInProgress{ false };
private:
	ComPtr<ID3D12CommandAllocator> uploadAllocator;
	ComPtr<ID3D12GraphicsCommandList> uploadCmdList;
	ComPtr<ID3D12Fence> uploadFence;
	uint64_t uploadFenceValue = 0;
	HANDLE uploadFenceEvent = nullptr;
private:
	ComPtr<ID3D12CommandAllocator> frameAllocators[Renderer::BackBufferCount];
	ComPtr<ID3D12GraphicsCommandList> frameCmdLists[Renderer::BackBufferCount];
	ComPtr<ID3D12Fence> frameFences[Renderer::BackBufferCount];
private:
	ThreadManager* threadManager;
	Renderer* renderer;
};