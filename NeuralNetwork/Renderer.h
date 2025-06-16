#pragma once
#include <array>
#include <cstdint>
#include <d3d12.h>
#include <d3dcommon.h>
#include <dxgi1_4.h>
#include <memory>
#include <string>
#include <unordered_map>
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

struct PipelineStateKey {
	uint64_t vsHash, psHash, dsHash, hsHash;

	bool operator==( const PipelineStateKey& other ) const
	{
		return vsHash == other.vsHash &&
			psHash == other.psHash &&
			dsHash == other.dsHash &&
			hsHash == other.hsHash;
	}
};

namespace std {
	template <>
	struct hash<PipelineStateKey> {
		size_t operator()( const PipelineStateKey& k ) const
		{
			// Combine hashes (boost::hash_combine style)
			size_t h = k.vsHash;
			h ^= k.psHash + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= k.dsHash + 0x9e3779b9 + (h << 6) + (h >> 2);
			h ^= k.hsHash + 0x9e3779b9 + (h << 6) + (h >> 2);
			return h;
		}
	};
}

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
	void RenderActorInstances( ID3D12GraphicsCommandList* cmdList,
							   const std::vector <PipelineStateKey>& pipelineStates,
							   const std::vector<D3D12_VERTEX_BUFFER_VIEW*>& vbViews,
							   const std::vector<D3D12_INDEX_BUFFER_VIEW*>& ibViews,
							   const std::vector<std::array<D3D12_GPU_VIRTUAL_ADDRESS, 3>>& shaderSlots,
							   const uint& instanceCount,
							   const uint& instanceBufferStart,
							   const bool& isStatic );
	void SetPipelineState( ID3D12GraphicsCommandList* cmdList, const PipelineStateKey& psoKey );
	void BindShaderSlots( ID3D12GraphicsCommandList* cmdList, D3D12_GPU_VIRTUAL_ADDRESS textureSlots, D3D12_GPU_VIRTUAL_ADDRESS vectorSlots, D3D12_GPU_VIRTUAL_ADDRESS scalarSlots );
	void BindSceneConstantBuffer( ID3D12GraphicsCommandList* cmdList, D3D12_GPU_VIRTUAL_ADDRESS sceneBuffer );
	ComPtr<ID3DBlob> LoadShaderBlob( const std::string& fileName );
	void CompileShader( const std::string& shaderCode, const ShaderType& type, const std::string& name, ComPtr<ID3DBlob>& blob );
	PipelineStateKey CreatePipelineState( ComPtr<ID3DBlob>& vsBlob, ComPtr<ID3DBlob>& psBlob, ComPtr<ID3DBlob>& dsBlob, ComPtr<ID3DBlob>& hsBlob );
private:
	void CreateRenderTargets();
	void CreateDepthStencils( uint width, uint height );
	bool ConfigureRootSignature();
	void BindConstantBuffer( ID3D12GraphicsCommandList* cmdList, const uint& slot, D3D12_GPU_VIRTUAL_ADDRESS buffer );
	void BindRootConstants( ID3D12GraphicsCommandList* cmdList, void* constants );
	void CleanupRenderTargets();
	uint64_t HashBlob( const void* data, size_t size );
	std::string GetLatestShaderModel();
	void RenderMeshInstances( ID3D12GraphicsCommandList* cmdList,
							  D3D12_VERTEX_BUFFER_VIEW* vbView,
							  D3D12_INDEX_BUFFER_VIEW* ibView,
							  const std::array<D3D12_GPU_VIRTUAL_ADDRESS, 3>& shaderSlots,
							  const uint& instanceCount );
private:
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12RootSignature> rootSignature;
	std::unordered_map<PipelineStateKey, ComPtr<ID3D12PipelineState>> pipelineStateCache;
	PipelineStateKey currentPipelineState;
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