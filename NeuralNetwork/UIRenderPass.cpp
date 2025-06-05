//#include <combaseapi.h>
//#include <d3d12.h>
//#include <Windows.h>
//#include <wrl\client.h>
//#include "DescriptorHeapManager.h"
//#include "GraphPass.h"
//#include "ImGuiLayer.h"
//#include "Renderer.h"
//#include "UIRenderPass.h"
//
//UIRenderPass::UIRenderPass( Renderer& renderer, ImGuiLayer* layer )
//	: GraphPass( "ImGuiPass" ), imguiLayer( layer )
//{
//	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
//	ID3D12Device* device = renderer.GetDevice();
//
//	// Create a command allocator for the command list
//	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
//	HRESULT hr = device->CreateCommandAllocator(
//		D3D12_COMMAND_LIST_TYPE_DIRECT,
//		IID_PPV_ARGS( &commandAllocator )
//	);
//
//	if( SUCCEEDED( hr ) ) {
//		// Create the command list
//		hr = device->CreateCommandList(
//			0,
//			D3D12_COMMAND_LIST_TYPE_DIRECT,
//			commandAllocator.Get(),
//			nullptr,
//			IID_PPV_ARGS( &commandList )
//		);
//
//		if( SUCCEEDED( hr ) ) {
//			// Close the command list initially (required before resetting)
//			commandList->Close();
//			AddCommandList( commandList, commandAllocator );
//		}
//	}
//}
//
//void UIRenderPass::Execute( Renderer& renderer )
//{
//	// Get the current command list and render target
//	auto* cmdList = commandLists[0].Get();
//	auto* allocator = commandAllocators[0].Get();
//
//	allocator->Reset(); // Reset the command allocator before use
//	cmdList->Reset( allocator, nullptr );
//
//	// Set the render target
//	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderer.GetCurrentRtvHandle();
//	cmdList->OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );
//
//	ID3D12DescriptorHeap* heaps[] = { renderer.GetSrvHeapManager()->GetHeap() };
//	cmdList->SetDescriptorHeaps( 1, heaps );
//
//	imguiLayer->BeginFrame();
//	imguiLayer->RenderUI();
//	imguiLayer->EndFrame( cmdList );
//	commandLists[0]->Close();
//}
//
//void UIRenderPass::Shutdown()
//{
//	imguiLayer->Shutdown();
//	commandLists.clear();
//	commandAllocators.clear();
//}