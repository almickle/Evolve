#include <combaseapi.h>
#include <cstdint>
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3dx12_core.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxgiformat.h>
#include <fstream>
#include <handleapi.h>
#include <memory>
#include <synchapi.h>
#include <vector>
#include <Windows.h>
#include <wrl\client.h>
#include "ExecutionGraph.h"
#include "Renderer.h"
#include "Types.h"
#include "Window.h"

Renderer::~Renderer()
{
	WaitForGpu();
	CleanupRenderTargets();

	if( swapChain ) {
		BOOL isFullscreen = FALSE;
		ComPtr<IDXGIOutput> pOutput;
		swapChain->GetFullscreenState( &isFullscreen, &pOutput );
		if( isFullscreen ) {
			swapChain->SetFullscreenState( FALSE, nullptr );
		}
		swapChain.Reset();
		pOutput.Reset();
	}
	CloseHandle( fenceEvent );
	fence.Reset();
	commandQueue.Reset();
	rtvHeap.Reset();
	device.Reset();

#if defined(_DEBUG)
	ComPtr<IDXGIDebug1> dxgiDebug;
	if( SUCCEEDED( DXGIGetDebugInterface1( 0, IID_PPV_ARGS( &dxgiDebug ) ) ) ) {
		dxgiDebug->ReportLiveObjects( DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL );
	}
#endif
}

bool Renderer::Init()
{
	uint dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugController;
	if( SUCCEEDED( D3D12GetDebugInterface( IID_PPV_ARGS( &debugController ) ) ) ) {
		debugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	ComPtr<IDXGIFactory6> factory;
	if( FAILED( CreateDXGIFactory2( dxgiFactoryFlags, IID_PPV_ARGS( &factory ) ) ) )
		return false;

	ComPtr<IDXGIAdapter1> adapter;
	for( uint adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapterByGpuPreference(
		adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS( &adapter ) ); ++adapterIndex )
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1( &desc );

		// Skip software adapters
		if( desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE )
			continue;

		// Try to create device
		if( SUCCEEDED( D3D12CreateDevice( adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS( &device ) ) ) )
			break;
	}

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue;
	if( SUCCEEDED( device.As( &infoQueue ) ) ) {
		infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_ERROR, true );
		infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_CORRUPTION, true );
		infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_WARNING, true );
	}
#endif

	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = BackBufferCount;
	if( FAILED( device->CreateDescriptorHeap( &rtvDesc, IID_PPV_ARGS( &rtvHeap ) ) ) )
		return false;

	SIZE_T rtvDescriptorSize = device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
	auto handle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
	for( uint i = 0; i < BackBufferCount; ++i ) {
		rtDescHandles[i] = handle;
		handle.ptr += rtvDescriptorSize;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	if( FAILED( device->CreateCommandQueue( &queueDesc, IID_PPV_ARGS( &commandQueue ) ) ) )
		return false;

	if( FAILED( device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &fence ) ) ) )
		return false;

	fenceEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );
	if( !fenceEvent )
		return false;

	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.BufferCount = BackBufferCount;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.SampleDesc.Count = 1;

	//DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};

	ComPtr<IDXGISwapChain1> tempSwapChain;
	if( FAILED( factory->CreateSwapChainForHwnd( commandQueue.Get(), window->GetHWND(), &scDesc, nullptr, nullptr, &tempSwapChain ) ) )
		return false;

	if( FAILED( tempSwapChain.As( &swapChain ) ) )
		return false;

	//swapChain->SetFullscreenState(TRUE, nullptr);

	RECT rect;
	GetClientRect( window->GetHWND(), &rect );
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	// Release old render targets if any
	CleanupRenderTargets();

	// Resize swap chain buffers to match the new mode
	swapChain->ResizeBuffers( BackBufferCount, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0 );

	// Recreate render targets for the new buffers
	CreateRenderTargets();

	// Create depth stencil buffer
	CreateDepthStencil( width, height );

	return true;
}

void Renderer::Present()
{
	// Gather all command lists from execution graphs
	std::vector<ID3D12CommandList*> allCmdLists;
	if( simulationGraph ) {
		auto graphCmdLists = simulationGraph->GetAllCommandLists( frameIndex );
		allCmdLists.insert( allCmdLists.end(), graphCmdLists.begin(), graphCmdLists.end() );
	}
	if( renderGraph ) {
		auto graphCmdLists = renderGraph->GetAllCommandLists( frameIndex );
		allCmdLists.insert( allCmdLists.end(), graphCmdLists.begin(), graphCmdLists.end() );
	}

	commandQueue->ExecuteCommandLists( static_cast<uint>(allCmdLists.size()), allCmdLists.data() );

	// Without V-Sync
	swapChain->Present( 0, 0 );

	WaitForGpu();
}

void Renderer::WaitForGpu()
{
	const uint64_t fenceToWait = ++currentFenceValue;
	commandQueue->Signal( fence.Get(), fenceToWait );
	if( fence->GetCompletedValue() < fenceToWait ) {
		fence->SetEventOnCompletion( fenceToWait, fenceEvent );
		WaitForSingleObject( fenceEvent, INFINITE );
	}
}

void Renderer::CreateRenderTargets()
{
	for( uint i = 0; i < BackBufferCount; ++i ) {
		ComPtr<ID3D12Resource> buffer;
		swapChain->GetBuffer( i, IID_PPV_ARGS( &buffer ) );
		device->CreateRenderTargetView( buffer.Get(), nullptr, rtDescHandles[i] );
		backBuffers[i] = buffer;
	}
}

void Renderer::CreateDepthStencil( uint width, uint height )
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = BackBufferCount;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap( &dsvHeapDesc, IID_PPV_ARGS( &dsvHeap ) );

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
	CD3DX12_HEAP_PROPERTIES heapProperties( D3D12_HEAP_TYPE_DEFAULT );

	// Use the address of the named variable
	device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS( &depthStencilBuffer )
	);

#if defined(_DEBUG)
	depthStencilBuffer->SetName( L"Depth Buffer" );
#endif

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	SIZE_T dsvDescriptorSize = device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_DSV );
	auto handle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	for( uint i = 0; i < BackBufferCount; ++i ) {
		dsvDescHandles[i] = handle;
		device->CreateDepthStencilView( depthStencilBuffer.Get(), &dsvDesc, handle );
		handle.ptr += dsvDescriptorSize;
	}
}

ComPtr<ID3DBlob> Renderer::LoadShaderBlob( const wchar_t* filename )
{
	std::ifstream file( filename, std::ios::binary | std::ios::ate );
	if( !file ) return nullptr;
	std::streamsize size = file.tellg();
	file.seekg( 0, std::ios::beg );
	ComPtr<ID3DBlob> blob;
	if( FAILED( D3DCreateBlob( static_cast<SIZE_T>(size), &blob ) ) ) return nullptr;
	file.read( reinterpret_cast<char*>(blob->GetBufferPointer()), size );
	return blob;
}

void Renderer::CleanupRenderTargets()
{
	for( uint i = 0; i < BackBufferCount; ++i ) {
		backBuffers[i].Reset();
	}
}