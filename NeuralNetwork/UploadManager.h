#pragma once
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <Windows.h>
#include <wrl/client.h>
#include "d3d12.h"

class Renderer; // Forward declaration

struct UploadRequest {
	std::function<void( ID3D12GraphicsCommandList* )> recordFunc;
	std::function<void()> onComplete; // Optional callback
};

class UploadManager {
public:
	UploadManager( Renderer& renderer );
	~UploadManager();
public:
	void Enqueue( UploadRequest req );
	void Shutdown();
	void Flush();
private:
	void StartUploadThread( Renderer& renderer );
private:
	std::queue<UploadRequest> queue;
	std::mutex mutex;
	std::condition_variable cv;
	std::thread uploadThread;
	std::atomic<bool> running{ true };
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> uploadAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> uploadCmdList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> uploadCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12Fence> uploadFence;
	uint64_t uploadFenceValue = 0;
	HANDLE uploadFenceEvent = nullptr;
};