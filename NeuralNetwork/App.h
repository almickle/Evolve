#pragma once
#include "ImGuiLayer.h"
#include "Renderer.h"
#include "Window.h"
#include "Scene.h"

struct InputState {
	bool keys[256] = {};
	int mouseDeltaX = 0;
	int mouseDeltaY = 0;
	int lastMouseX = 0;
	int lastMouseY = 0;
	bool firstMouse = true;
};

class App {
public:
	bool Init(HINSTANCE hInstance, int nCmdShow);
	void Run();
	void UpdateInputState();
	void UpdateCameraFromInput();
	void Shutdown();
	std::shared_ptr<RenderGraph> BuildRenderGraph(Renderer& renderer, ImGuiLayer& imguiLayer);
private:
	Window window;
	Renderer renderer;
	ImGuiLayer imgui;
	Scene scene;
	InputState inputState;
};