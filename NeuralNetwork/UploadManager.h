#pragma once
#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <Windows.h>
#include <wrl/client.h>
#include "d3d12.h"
#include "ThreadManager.h"

class Renderer; // Forward declaration

struct UploadRequest {
	std::function<void( ID3D12GraphicsCommandList* )> recordFunc;
	std::function<void()> onComplete; // Optional callback
};

class UploadManager {
public:
	UploadManager( Renderer& renderer );
	~UploadManager() = default;
public:
	void Enqueue( UploadRequest req );
	void Flush();
private:
private:
private:
	ThreadManager* threadManager = nullptr;
	std::mutex queueMutex;
	std::queue<UploadRequest> uploadQueue;
	std::atomic<bool> batchInProgress{ false };
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> uploadAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> uploadCmdList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> uploadCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12Fence> uploadFence;
	uint64_t uploadFenceValue = 0;
	HANDLE uploadFenceEvent = nullptr;
};