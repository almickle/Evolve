#include <d3d12.h>
#include <dxgiformat.h>
#include <imgui_node_editor.h>
#include "DescriptorHeapManager.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "ImGuiLayer.h"
#include "Renderer.h"
#include "UiNode.h"
#include "Window.h"

ImGuiLayer::~ImGuiLayer()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiLayer::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	ImGui::StyleColorsDark();

	imguiFontSrvIndex = srvHeapManager->Allocate();
	D3D12_CPU_DESCRIPTOR_HANDLE imguiFontCpuHandle = srvHeapManager->GetCpuHandle( imguiFontSrvIndex );
	D3D12_GPU_DESCRIPTOR_HANDLE imguiFontGpuHandle = srvHeapManager->GetGpuHandle( imguiFontSrvIndex );

	ImGui_ImplWin32_Init( window->GetHWND() );

	ImGui_ImplDX12_InitInfo init_info = {};
	init_info.Device = renderer->GetDevice();
	init_info.CommandQueue = renderer->GetCommandQueue();
	init_info.NumFramesInFlight = Renderer::BackBufferCount;
	init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
	init_info.UserData = srvHeapManager;
	// Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
	// (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
	init_info.SrvDescriptorHeap = srvHeapManager->GetHeap();
	init_info.SrvDescriptorAllocFn = [](
		ImGui_ImplDX12_InitInfo* initInfo,
		D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle,
		D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle )
		{
			return static_cast<DescriptorHeapManager*>(initInfo->UserData)->Alloc( out_cpu_handle, out_gpu_handle );
		};
	init_info.SrvDescriptorFreeFn = [](
		ImGui_ImplDX12_InitInfo* initInfo,
		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle )
		{
			return static_cast<DescriptorHeapManager*>(initInfo->UserData)->Free( cpu_handle, gpu_handle );
		};

	ImGui_ImplDX12_Init( &init_info );
}

void ImGuiLayer::BeginFrame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiLayer::RenderUI()
{
	if( ImGui::BeginMainMenuBar() )
	{
		if( ImGui::BeginMenu( "File" ) )
		{
			if( ImGui::MenuItem( "Import" ) ) { /* ... */ }
			if( ImGui::MenuItem( "Export" ) ) { /* ... */ }
			if( ImGui::MenuItem( "Save" ) ) { /* ... */ }
			ImGui::EndMenu();
		}

		if( ImGui::BeginMenu( "Open" ) )
		{
			if( ImGui::MenuItem( "Shader Editor" ) )
			{
				SetContext( UiContextType::ShaderEditor );
			}
			if( ImGui::MenuItem( "Model Viewer" ) ) { /* ... */ }
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if( uiContext.currentContext == UiContextType::ShaderEditor )
	{
		shaderEditor.Render();
	}
}


void ImGuiLayer::EndFrame( ID3D12GraphicsCommandList* commandList )
{
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData( ImGui::GetDrawData(), commandList );
}

bool ImGuiLayer::UsingInput() const
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

