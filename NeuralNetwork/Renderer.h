#pragma once
#include <cstdint>
#include <d3d12.h>
#include <d3dcommon.h>
#include <dxgi1_4.h>
#include <memory>
#include <Windows.h>
#include <wrl\client.h>
#include "DescriptorHeapManager.h"
#include "ExecutionGraph.h"
#include "GpuResourceManager.h"
#include "ThreadManager.h"
#include "UploadManager.h"

using Microsoft::WRL::ComPtr;
using uint64 = uint64_t;

class ExecutionGraph;

class Renderer {
public:
	bool Init( HWND hwnd );
	void Present();
	void Shutdown();
	void WaitForGpu();
	void CreateRenderTargets();
	void CleanupRenderTargets();
	void CreateDepthStencil( uint width, uint height );
public:
	void SetInitializationGraph( std::shared_ptr<ExecutionGraph> graph ) { initializationGraph = graph; };
	void SetSimulationGraph( std::shared_ptr<ExecutionGraph> graph ) { simulationGraph = graph; };
	void SetRenderGraph( std::shared_ptr<ExecutionGraph> graph ) { renderGraph = graph; };
	void UpdateCurrentFrameIndex() { frameIndex = swapChain->GetCurrentBackBufferIndex(); };
public:
	ExecutionGraph* GetInitializationGraph() const { return initializationGraph.get(); }
	ExecutionGraph* GetSimulationGraph() const { return simulationGraph.get(); }
	ExecutionGraph* GetRenderGraph() const { return renderGraph.get(); }
	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue.Get(); }
	uint GetWidth() const { return width; }
	uint GetHeight() const { return height; }
public:
	ID3D12Resource* GetCurrentBackBuffer() const { return backBuffers[frameIndex].Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvHandle() const { return rtDescHandles[frameIndex]; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDsvHandle() const { return dsvDescHandles[frameIndex]; }
	uint GetCurrentFrameIndex() const { return frameIndex; }
public:
	ComPtr<ID3DBlob> LoadShaderBlob( const wchar_t* filename ); // move to utils
public:
	ThreadManager* GetThreadManager() const { return threadManager.get(); }
	UploadManager* GetUploadManager() const { return uploadManager.get(); }
	DescriptorHeapManager* GetSrvHeapManager() const { return srvHeapManager.get(); }
	GpuResourceManager* GetGpuResourceManager() const { return gpuResourceManager.get(); }
public:
	static const uint BackBufferCount = 3;
private:
	D3D12_CPU_DESCRIPTOR_HANDLE rtDescHandles[BackBufferCount];
	D3D12_CPU_DESCRIPTOR_HANDLE dsvDescHandles[BackBufferCount];
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12Resource> backBuffers[3];
	ComPtr<ID3D12Resource> depthStencilBuffer;
	ComPtr<ID3D12Fence> fence;
	uint frameIndex = 0;
	HANDLE fenceEvent = nullptr;
	uint64 currentFenceValue = 0;
	uint width = 0;
	uint height = 0;
private:
	std::shared_ptr<ExecutionGraph> initializationGraph;
	std::shared_ptr<ExecutionGraph> simulationGraph;
	std::shared_ptr<ExecutionGraph> renderGraph;
	std::unique_ptr<UploadManager> uploadManager;
	std::unique_ptr<GpuResourceManager> gpuResourceManager;
	std::unique_ptr<DescriptorHeapManager> srvHeapManager;
	std::unique_ptr<ThreadManager> threadManager;
};