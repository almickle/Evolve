#include "Renderer.h"
#include "RenderGraph.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <d3dcompiler.h>

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
		if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocatorsBegin[i]))))
			return false;
	}

	for (UINT i = 0; i < BackBufferCount; ++i) {
		if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocatorsEnd[i]))))
			return false;
	}

	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocatorsBegin[0].Get(), nullptr, IID_PPV_ARGS(&commandListBegin))))
		return false;
	commandListBegin->Close();

	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocatorsEnd[0].Get(), nullptr, IID_PPV_ARGS(&commandListEnd))))
		return false;
	commandListEnd->Close();

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

	//DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};

	ComPtr<IDXGISwapChain1> tempSwapChain;
	if (FAILED(factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &scDesc, nullptr, nullptr, &tempSwapChain)))
		return false;

	if (FAILED(tempSwapChain.As(&swapChain)))
		return false;

	//swapChain->SetFullscreenState(TRUE, nullptr);

	RECT rect;
	GetClientRect(hwnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	// Release old render targets if any
	CleanupRenderTargets();

	// Resize swap chain buffers to match the new mode
	swapChain->ResizeBuffers(BackBufferCount, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// Recreate render targets for the new buffers
	CreateRenderTargets();

	// Create depth stencil buffer
	CreateDepthStencil(width, height);

	D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
	srvDesc.NumDescriptors = 100;
	srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	if (FAILED(device->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(&srvHeap))))
		return false;

	#if defined(_DEBUG)
		srvHeap->SetName(L"SRV Heap");
	#endif

	return true;
}

ComPtr<ID3DBlob> Renderer::LoadShaderBlob(const wchar_t* filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (!file) return nullptr;
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	ComPtr<ID3DBlob> blob;
	if (FAILED(D3DCreateBlob(static_cast<SIZE_T>(size), &blob))) return nullptr;
	file.read(reinterpret_cast<char*>(blob->GetBufferPointer()), size);
	return blob;
}

void Renderer::BeginFrame()
{
    frameIndex = swapChain->GetCurrentBackBufferIndex();

    commandAllocatorsBegin[frameIndex]->Reset();
    commandListBegin->Reset(commandAllocatorsBegin[frameIndex].Get(), nullptr);

    // Transition the back buffer to RENDER_TARGET
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = backBuffers[frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandListBegin->ResourceBarrier(1, &barrier);

    // Set render target and depth stencil view
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtDescHandles[frameIndex];
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescHandles[frameIndex];
    commandListBegin->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // Clear the render target and depth buffer
    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // RGBA
    commandListBegin->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandListBegin->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    commandListBegin->SetDescriptorHeaps(1, srvHeap.GetAddressOf());

    commandListBegin->Close();

    // Execute the render graph
    if (renderGraph)
        renderGraph->Execute(*this);
}

void Renderer::SetRenderGraph(std::shared_ptr<RenderGraph> graph) {
	renderGraph = graph;
}

void Renderer::EndFrame()
{
	commandAllocatorsEnd[frameIndex]->Reset();
	commandListEnd->Reset(commandAllocatorsEnd[frameIndex].Get(), nullptr);

    // Transition the back buffer to PRESENT
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = backBuffers[frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandListEnd->ResourceBarrier(1, &barrier);

	commandListEnd->Close();

    // Gather all command lists: the main command list and all from the render graph
    std::vector<ID3D12CommandList*> allCmdLists;
	allCmdLists.push_back(commandListBegin.Get());
    if (renderGraph) {
        auto graphCmdLists = renderGraph->GetAllCommandLists();

        // Add a resource barrier and close each render pass command list
        for (size_t i = 0; i < graphCmdLists.size(); ++i) {
            auto* cmdList = static_cast<ID3D12GraphicsCommandList*>(graphCmdLists[i]);
            cmdList->Close();
        }

        allCmdLists.insert(allCmdLists.end(), graphCmdLists.begin(), graphCmdLists.end());
    }
	allCmdLists.push_back(commandListEnd.Get());

    commandQueue->ExecuteCommandLists(static_cast<UINT>(allCmdLists.size()), allCmdLists.data());

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

void Renderer::CreateDepthStencil(UINT width, UINT height) {
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = BackBufferCount;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    D3D12_RESOURCE_DESC depthDesc = {};
    depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.DepthOrArraySize = 1;
    depthDesc.MipLevels = 1;
    depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    // Create a named variable for the heap properties
    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

    // Use the address of the named variable
    device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&depthStencilBuffer)
    );

	#if defined(_DEBUG)
		depthStencilBuffer->SetName(L"Depth Buffer");
	#endif

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

    SIZE_T dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    auto handle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < BackBufferCount; ++i) {
        dsvDescHandles[i] = handle;
        device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, handle);
        handle.ptr += dsvDescriptorSize;
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
	commandListBegin.Reset();
	commandListEnd.Reset();
	for (auto& allocator : commandAllocatorsBegin) allocator.Reset();
	for (auto& allocator : commandAllocatorsEnd) allocator.Reset();
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
