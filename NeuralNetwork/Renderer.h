#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <memory>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class RenderGraph;

class Renderer {
public:
	bool Init(HWND hwnd);
	void BeginFrame();
	void EndFrame();
	void Shutdown();
	void WaitForGpu();
	void CreateRenderTargets();
	void CleanupRenderTargets();
	void SetRenderGraph(std::shared_ptr<RenderGraph> graph);

public:
	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandListBegin() const { return commandListBegin.Get(); }
	ID3D12GraphicsCommandList* GetCommandListEnd() const { return commandListEnd.Get(); }
	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue.Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvHandle() const { return rtDescHandles[frameIndex]; }
	ID3D12DescriptorHeap* GetSrvHeap() const { return srvHeap.Get(); }
	UINT GetCurrentFrameIndex() const { return frameIndex; }

private:
	static const UINT BackBufferCount = 3;
	D3D12_CPU_DESCRIPTOR_HANDLE rtDescHandles[BackBufferCount];
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandAllocator> commandAllocatorsBegin[BackBufferCount];
	ComPtr<ID3D12CommandAllocator> commandAllocatorsEnd[BackBufferCount];
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12GraphicsCommandList> commandListBegin;
	ComPtr<ID3D12GraphicsCommandList> commandListEnd;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12Resource> backBuffers[3];
	ComPtr<ID3D12Fence> fence;
	UINT frameIndex = 0;
	HANDLE fenceEvent = nullptr;
	UINT64 currentFenceValue = 0;
private:
	std::shared_ptr<RenderGraph> renderGraph;
};