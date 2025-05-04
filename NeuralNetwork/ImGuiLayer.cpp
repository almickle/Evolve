#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "ImGuiLayer.h"

void ImGuiLayer::Init(HWND hwnd, Renderer& renderer)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(
		renderer.GetDevice(), 3,
		DXGI_FORMAT_R8G8B8A8_UNORM, renderer.GetSrvHeap(),
		renderer.GetSrvHeap()->GetCPUDescriptorHandleForHeapStart(),
		renderer.GetSrvHeap()->GetGPUDescriptorHandleForHeapStart()
	);
}

void ImGuiLayer::BeginFrame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiLayer::RenderUI()
{
	ImGui::ShowDemoWindow();
}


void ImGuiLayer::EndFrame(ID3D12GraphicsCommandList* commandList)
{
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void ImGuiLayer::Shutdown()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
