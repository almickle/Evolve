#include "Renderer.h"
#include "RenderGraph.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>

bool Renderer::Init(HWND hwnd)
{
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	ComPtr<IDXGIFactory6> factory;
	if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory))))
		return false;

	ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapterByGpuPreference(
		adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		// Skip software adapters
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		// Try to create device
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
			break;
	}

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(device.As(&infoQueue))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	}
#endif

	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = BackBufferCount;
	if (FAILED(device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&rtvHeap))))
		return false;

	SIZE_T rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	auto handle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < BackBufferCount; ++i) {
		rtDescHandles[i] = handle;
		handle.ptr += rtvDescriptorSize;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
		return false;

	for (UINT i = 0; i < BackBufferCount; ++i) {
		if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i]))))
			return false;
	}

	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&commandList))))
		return false;
	commandList->Close();

	if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
		return false;

	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!fenceEvent)
		return false;


	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.BufferCount = BackBufferCount;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> tempSwapChain;
	if (FAILED(factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &scDesc, nullptr, nullptr, &tempSwapChain)))
		return false;

	if (FAILED(tempSwapChain.As(&swapChain)))
		return false;

	swapChain->SetFullscreenState(TRUE, nullptr);

	RECT rect;
	GetClientRect(hwnd, &rect);
	UINT width = rect.right - rect.left;
	UINT height = rect.bottom - rect.top;

	// Release old render targets if any
	CleanupRenderTargets();

	// Resize swap chain buffers to match the new mode
	swapChain->ResizeBuffers(BackBufferCount, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// Recreate render targets for the new buffers
	CreateRenderTargets();

	D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
	srvDesc.NumDescriptors = 100;
	srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	if (FAILED(device->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(&srvHeap))))
		return false;

	return true;
}

void Renderer::BeginFrame()
{
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	commandAllocators[frameIndex]->Reset();
	commandList->Reset(commandAllocators[frameIndex].Get(), nullptr);

	// Transition the back buffer to RENDER_TARGET
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = backBuffers[frameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	// Set render target
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtDescHandles[frameIndex];
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Clear the render target
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // RGBA
	commandList->OMSetRenderTargets(1, &rtDescHandles[frameIndex], FALSE, nullptr);
	commandList->ClearRenderTargetView(rtDescHandles[frameIndex], clearColor, 0, nullptr);
	commandList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());

	// Execute the render graph
	if (renderGraph)
		renderGraph->Execute(*this);
}

void Renderer::SetRenderGraph(std::shared_ptr<RenderGraph> graph) {
	renderGraph = graph;
}

void Renderer::EndFrame()
{
	// Transition the back buffer to PRESENT
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = backBuffers[frameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrier);

	commandList->Close();

	ID3D12CommandList* cmdsLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(1, cmdsLists);
	// Without V-Sync
	swapChain->Present(0, 0);
	// With V-Sync
	//swapChain->Present(1, 0);

	const UINT64 fenceToWait = ++currentFenceValue;
	commandQueue->Signal(fence.Get(), fenceToWait);
	if (fence->GetCompletedValue() < fenceToWait) {
		fence->SetEventOnCompletion(fenceToWait, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void Renderer::WaitForGpu()
{
	const UINT64 fenceToWait = ++currentFenceValue;
	commandQueue->Signal(fence.Get(), fenceToWait);
	fence->SetEventOnCompletion(fenceToWait, fenceEvent);
	WaitForSingleObject(fenceEvent, INFINITE);
}

void Renderer::CreateRenderTargets()
{
	for (UINT i = 0; i < BackBufferCount; ++i) {
		ComPtr<ID3D12Resource> buffer;
		swapChain->GetBuffer(i, IID_PPV_ARGS(&buffer));
		device->CreateRenderTargetView(buffer.Get(), nullptr, rtDescHandles[i]);
		backBuffers[i] = buffer;
	}
}

void Renderer::CleanupRenderTargets()
{
	for (UINT i = 0; i < BackBufferCount; ++i) {
		backBuffers[i].Reset();
	}
}

void Renderer::Shutdown()
{
	WaitForGpu();
	CleanupRenderTargets();

	renderGraph->Shutdown();

	if (swapChain) {
		BOOL isFullscreen = FALSE;
		ComPtr<IDXGIOutput> pOutput;
		swapChain->GetFullscreenState(&isFullscreen, &pOutput);
		if (isFullscreen) {
			swapChain->SetFullscreenState(FALSE, nullptr);
		}
		swapChain.Reset();
		pOutput.Reset();
	}
	CloseHandle(fenceEvent);
	fence.Reset();
	commandList.Reset();
	for (auto& allocator : commandAllocators) allocator.Reset();
	commandQueue.Reset();
	srvHeap.Reset();
	rtvHeap.Reset();
	device.Reset();

#if defined(_DEBUG)
	ComPtr<IDXGIDebug1> dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	}
#endif
}
