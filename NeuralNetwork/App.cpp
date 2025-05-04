#include "App.h"

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
	if (!window.Create(L"DX12 ImGui App", 1280, 800, hInstance, nCmdShow))
		return false;

	if (!renderer.Init(window.GetHWND()))
		return false;

	imgui.Init(window.GetHWND(), renderer);
	return true;
}

void App::Run()
{
	while (!window.ShouldClose()) {
		window.PollEvents();

		renderer.BeginFrame();
		imgui.BeginFrame();

		imgui.RenderUI();

		imgui.EndFrame(renderer.GetCommandList());
		renderer.EndFrame();
	}
}

void App::Shutdown()
{
	imgui.Shutdown();
	renderer.Shutdown();
	window.Destroy();
}