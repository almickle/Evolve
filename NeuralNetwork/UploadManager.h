#pragma once
#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <Windows.h>
#include <wrl/client.h>
#include "d3d12.h"
#include "System.h"
#include "SystemManager.h"

class ThreadManager;
class Renderer;

struct UploadRequest {
	std::function<void( ID3D12GraphicsCommandList* )> recordFunc;
	std::function<void()> onComplete; // Optional callback
};

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
private:
	std::mutex queueMutex;
	std::queue<UploadRequest> uploadQueue;
	std::atomic<bool> batchInProgress{ false };
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> uploadAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> uploadCmdList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> uploadCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12Fence> uploadFence;
	uint64_t uploadFenceValue = 0;
	HANDLE uploadFenceEvent = nullptr;
private:
	ThreadManager* threadManager;
	Renderer* renderer;
};