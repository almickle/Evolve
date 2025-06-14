#pragma once
#include <cstdint>
#include <d3d12.h>
#include <d3dcommon.h>
#include <dxgi1_4.h>
#include <memory>
#include <vector>
#include <Windows.h>
#include <wrl\client.h>
#include "System.h"
#include "SystemManager.h"
#include "Types.h"

using Microsoft::WRL::ComPtr;

class ExecutionGraph;
class GpuResourceManager;
class DescriptorHeapManager;
class ThreadManager;
class UploadManager;
class Window;

class Renderer : public System {
public:
	Renderer( SystemManager& systemManager );
	~Renderer();
public:
	static const uint BackBufferCount = 3;
public:
	bool Init();
public:
	void Present();
	void WaitForCurrentFrame();
public:
	void UpdateCurrentFrameIndex() { frameIndex = swapChain->GetCurrentBackBufferIndex(); };
public:
	ExecutionGraph* GetRenderGraph() const { return renderGraph.get(); }
	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue.Get(); }
	uint GetWidth() const { return width; }
	uint GetHeight() const { return height; }
	ID3D12Resource* GetCurrentBackBuffer() const { return backBuffers[frameIndex].Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvHandle() const { return rtDescHandles[frameIndex]; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDsvHandle() const { return dsvDescHandles[frameIndex]; }
	uint GetCurrentFrameIndex() const { return frameIndex; }
	ID3D12RootSignature* GetRootSignature() const { return rootSignature.Get(); }
	D3D12_PRIMITIVE_TOPOLOGY GetTopology() const { return topology; }
public:
	void RenderMeshInstances( ID3D12GraphicsCommandList* cmdList, D3D12_VERTEX_BUFFER_VIEW* vbView, D3D12_INDEX_BUFFER_VIEW* ibView, const uint& instanceCount, const uint& instanceIndex );
	void BindMaterial( ID3D12GraphicsCommandList* cmdList, D3D12_GPU_VIRTUAL_ADDRESS textureSlots, D3D12_GPU_VIRTUAL_ADDRESS vectorSlots, D3D12_GPU_VIRTUAL_ADDRESS scalarSlots );
	void SetPipelineState( ID3D12GraphicsCommandList* cmdList, const uint& index );
	void BindSceneData( ID3D12GraphicsCommandList* cmdList, D3D12_GPU_VIRTUAL_ADDRESS sceneBuffer );
	ComPtr<ID3DBlob> LoadShaderBlob( const wchar_t* filename );
	void CreateRenderTargets();
	void CreateDepthStencils( uint width, uint height );
	bool ConfigureRootSignature();
	uint CreatePipelineState( ID3DBlob* vsBlob, ID3DBlob* psBlob, ID3DBlob* dsBlob = nullptr, ID3DBlob* hsBlob = nullptr );
	void CleanupRenderTargets();
private:
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12RootSignature> rootSignature;
	std::vector<ComPtr<ID3D12PipelineState>> pipelineStates{ 16 };
	D3D12_PRIMITIVE_TOPOLOGY topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
private:
	ComPtr<ID3D12Resource> backBuffers[BackBufferCount];
	ComPtr<ID3D12Resource> depthStencilBuffers[BackBufferCount];
	D3D12_CPU_DESCRIPTOR_HANDLE rtDescHandles[BackBufferCount]{};
	D3D12_CPU_DESCRIPTOR_HANDLE dsvDescHandles[BackBufferCount]{};
	ComPtr<ID3D12Fence> frameFences[BackBufferCount];
	uint64_t frameFenceValues[BackBufferCount]{ 0 };
	HANDLE frameFenceEvents[BackBufferCount]{ nullptr };
private:
	uint frameIndex = 0;
	uint width = 0;
	uint height = 0;
private:
	std::unique_ptr<ExecutionGraph> renderGraph;
private:
	Window* window;
};