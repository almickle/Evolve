#pragma once
#include "ImGuiLayer.h"
#include "Renderer.h"
#include "Window.h"

class App {
public:
	bool Init(HINSTANCE hInstance, int nCmdShow);
	void Run();
	void Shutdown();
	std::shared_ptr<RenderGraph> BuildRenderGraph(Renderer& renderer, ImGuiLayer& imguiLayer);
private:
	Window window;
	Renderer renderer;
	ImGuiLayer imgui;
};