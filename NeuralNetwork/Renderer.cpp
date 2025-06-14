#include <climits>
#include <combaseapi.h>
#include <cstdint>
#include <cstdlib>
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3dx12_core.h>
#include <d3dx12_default.h>
#include <d3dx12_root_signature.h>
#include <debugapi.h>
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
#include "SystemManager.h"
#include "Types.h"
#include "Window.h"

Renderer::Renderer( SystemManager& systemManager )
	: window( systemManager.GetWindow() ),
	renderGraph( std::make_unique<ExecutionGraph>() )
{
};

Renderer::~Renderer()
{
	WaitForCurrentFrame();
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
	for( uint i = 0; i < BackBufferCount; i++ )
	{
		CloseHandle( frameFenceEvents[i] );
		frameFences[i].Reset();
	}
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

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = BackBufferCount;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if( FAILED( device->CreateDescriptorHeap( &dsvHeapDesc, IID_PPV_ARGS( &dsvHeap ) ) ) )
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

	for( uint i = 0; i < BackBufferCount; i++ )
	{
		if( FAILED( device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &frameFences[i] ) ) ) )
			return false;

		frameFenceValues[i] = 1;
		frameFenceEvents[i] = CreateEvent( nullptr, FALSE, FALSE, nullptr );
		if( !frameFenceEvents[i] )
			return false;
	}


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
	CreateDepthStencils( width, height );

	return true;
}

void Renderer::Present()
{
	// Gather all command lists from execution graphs
	std::vector<ID3D12CommandList*> allCmdLists;
	if( renderGraph ) {
		auto graphCmdLists = renderGraph->GetAllCommandLists( frameIndex );
		allCmdLists.insert( allCmdLists.end(), graphCmdLists.begin(), graphCmdLists.end() );
	}

	commandQueue->ExecuteCommandLists( static_cast<uint>(allCmdLists.size()), allCmdLists.data() );

	// Without V-Sync
	swapChain->Present( 0, 0 );
}

void Renderer::WaitForCurrentFrame()
{
	const uint64_t fenceToWait = ++frameFenceValues[frameIndex];
	commandQueue->Signal( frameFences[frameIndex].Get(), fenceToWait );
	if( frameFences[frameIndex]->GetCompletedValue() < fenceToWait ) {
		frameFences[frameIndex]->SetEventOnCompletion( fenceToWait, frameFenceEvents[frameIndex] );
		WaitForSingleObject( frameFenceEvents[frameIndex], INFINITE );
	}
}

void Renderer::CreateRenderTargets()
{
	for( uint i = 0; i < BackBufferCount; i++ ) {
		ComPtr<ID3D12Resource> buffer;
		swapChain->GetBuffer( i, IID_PPV_ARGS( &buffer ) );
		device->CreateRenderTargetView( buffer.Get(), nullptr, rtDescHandles[i] );
		backBuffers[i] = buffer;
	}
}

void Renderer::CreateDepthStencils( uint width, uint height )
{
	for( uint i = 0; i < BackBufferCount; i++ )
	{

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
			IID_PPV_ARGS( &depthStencilBuffers[i] )
		);

		depthStencilBuffers[i]->SetName( L"Depth Buffer" );
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	SIZE_T dsvDescriptorSize = device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_DSV );
	auto handle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	for( uint i = 0; i < BackBufferCount; ++i ) {
		dsvDescHandles[i] = handle;
		device->CreateDepthStencilView( depthStencilBuffers[i].Get(), &dsvDesc, handle );
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

bool Renderer::ConfigureRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE1 ranges[2]{};
	ranges[1].Init(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
		2,			  // Num of srvs
		0,            // Base shader register t0
		1,            // Register space
		D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE,
		0             // Offset in descriptors
	);
	ranges[0].Init(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
		UINT_MAX,     // Or a reasonable maximum like 10000
		0,            // Base shader register t0
		0,            // Register space
		D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE,
		2             // Offset in descriptors
	);

	CD3DX12_ROOT_PARAMETER1 rootParameters[6]{};
	rootParameters[0].InitAsConstants( 8, 0 );
	rootParameters[1].InitAsConstantBufferView( 1 ); // Light and camera data
	rootParameters[2].InitAsConstantBufferView( 2 ); // Material texture slots
	rootParameters[3].InitAsConstantBufferView( 3 ); // Material vector slots
	rootParameters[4].InitAsConstantBufferView( 4 ); // Material scalar slots
	rootParameters[5].InitAsDescriptorTable( 2, ranges, D3D12_SHADER_VISIBILITY_ALL );

	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.MipLODBias = 0.0f;
	sampler.MaxAnisotropy = 1;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0; // register(s0)
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.Init_1_1(
		_countof( rootParameters ), rootParameters,
		1, &sampler, // <-- Add static sampler here
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3D12SerializeVersionedRootSignature(
		&rootSigDesc, &signatureBlob, &errorBlob
	);
	if( FAILED( hr ) ) {
		if( errorBlob ) {
			OutputDebugStringA( (char*)errorBlob->GetBufferPointer() );
		}
		return false;
	}

	hr = device->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS( &rootSignature )
	);
	if( FAILED( hr ) ) return false;
	// ... rest of your Init() code ...
	return true;
}

uint Renderer::CreatePipelineState( ID3DBlob* vsBlob, ID3DBlob* psBlob, ID3DBlob* dsBlob, ID3DBlob* hsBlob )
{
	D3D12_INPUT_ELEMENT_DESC inputLayout[4] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "UV",       0, DXGI_FORMAT_R32G32_FLOAT,    0, 24,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
	psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
	psoDesc.DS = dsBlob ? D3D12_SHADER_BYTECODE{ dsBlob->GetBufferPointer(), dsBlob->GetBufferSize() } : D3D12_SHADER_BYTECODE{ nullptr, 0 };
	psoDesc.HS = hsBlob ? D3D12_SHADER_BYTECODE{ hsBlob->GetBufferPointer(), hsBlob->GetBufferSize() } : D3D12_SHADER_BYTECODE{ nullptr, 0 };
	psoDesc.InputLayout = { inputLayout, _countof( inputLayout ) };
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC( D3D12_DEFAULT );
	psoDesc.BlendState = CD3DX12_BLEND_DESC( D3D12_DEFAULT );
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC( D3D12_DEFAULT );
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	ComPtr<ID3D12PipelineState> pso;
	device->CreateGraphicsPipelineState( &psoDesc, IID_PPV_ARGS( &pso ) );
	pipelineStates.push_back( pso );

	return (uint)pipelineStates.size() - 1;
}

void Renderer::BindSceneData( ID3D12GraphicsCommandList* cmdList, D3D12_GPU_VIRTUAL_ADDRESS sceneBuffer )
{
	cmdList->SetGraphicsRootConstantBufferView( 1, sceneBuffer );
}

void Renderer::SetPipelineState( ID3D12GraphicsCommandList* cmdList, const uint& index )
{
	cmdList->SetPipelineState( pipelineStates[index].Get() );
}

void Renderer::BindMaterial( ID3D12GraphicsCommandList* cmdList, D3D12_GPU_VIRTUAL_ADDRESS textureSlots, D3D12_GPU_VIRTUAL_ADDRESS vectorSlots, D3D12_GPU_VIRTUAL_ADDRESS scalarSlots )
{
	cmdList->SetGraphicsRootConstantBufferView( 2, textureSlots );
	cmdList->SetGraphicsRootConstantBufferView( 3, vectorSlots );
	cmdList->SetGraphicsRootConstantBufferView( 4, scalarSlots );
}

void Renderer::RenderMeshInstances( ID3D12GraphicsCommandList* cmdList, D3D12_VERTEX_BUFFER_VIEW* vbView, D3D12_INDEX_BUFFER_VIEW* ibView, const uint& instanceCount, const uint& instanceIndex )
{
	// Set by mesh instance
	cmdList->SetGraphicsRoot32BitConstants( 0, 2, &instanceIndex, 0 );
	cmdList->IASetVertexBuffers( 0, 1, vbView );
	cmdList->IASetIndexBuffer( ibView );

	UINT indexCount = 0;
	if( ibView->Format == DXGI_FORMAT_R16_UINT ) {
		indexCount = ibView->SizeInBytes / sizeof( uint16_t );
	}
	else if( ibView->Format == DXGI_FORMAT_R32_UINT ) {
		indexCount = ibView->SizeInBytes / sizeof( uint32_t );
	}
	else {
		throw("Index format not supported. Supported formats are: uint32, uint16");
	}

	cmdList->DrawIndexedInstanced( indexCount, instanceCount, 0, 0, 0 );
}