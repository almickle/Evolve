#include "App.h"
#include "ImGuiRenderPass.h"
#include "RenderGraph.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	App app;
	if (!app.Init(hInstance, nCmdShow))
		return -1;

	app.Run();
	app.Shutdown();
	return 0;
}

bool App::Init(HINSTANCE hInstance, int nCmdShow)
{
	if (!window.Create(L"DX12 ImGui App", hInstance, nCmdShow))
		return false;

	if (!renderer.Init(window.GetHWND()))
		return false;

	imgui.Init(window.GetHWND(), renderer);

	auto graph = BuildRenderGraph(renderer, imgui);
	renderer.SetRenderGraph(graph);

	return true;
}

std::shared_ptr<RenderGraph> App::BuildRenderGraph(Renderer& renderer, ImGuiLayer& imguiLayer) {
	auto graph = std::make_shared<RenderGraph>();

	auto imguiPass = std::make_shared<ImGuiRenderPass>(&imguiLayer);

	// Add other passes and dependencies here as you expand the system

	graph->AddPass(imguiPass);
	return graph;
}

void App::Run()
{
	while (!window.ShouldClose()) {
		window.PollEvents();

		renderer.BeginFrame();
		//imgui.BeginFrame();

		//imgui.RenderUI();

		//imgui.EndFrame(renderer.GetCommandList());
		renderer.EndFrame();
	}
}

void App::Shutdown()
{
	imgui.Shutdown();
	renderer.Shutdown();
	window.Destroy();
}