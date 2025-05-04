#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class Renderer {
public:
	bool Init(HWND hwnd);
	void BeginFrame();
	void EndFrame();
	void Shutdown();
	void WaitForGpu();
	void CreateRenderTargets();
	void CleanupRenderTargets();

public:
	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
	ID3D12DescriptorHeap* GetSrvHeap() const { return srvHeap.Get(); }

private:
	static const UINT BackBufferCount = 3;
	D3D12_CPU_DESCRIPTOR_HANDLE rtDescHandles[BackBufferCount];

	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandAllocator> commandAllocators[BackBufferCount];
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12Resource> backBuffers[3];
	ComPtr<ID3D12Fence> fence;
	UINT frameIndex = 0;
	HANDLE fenceEvent = nullptr;
	UINT64 currentFenceValue = 0;

};