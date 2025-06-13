#pragma once
#include <memory>
#include <Windows.h>
#include "ExecutionGraph.h"
#include "ImGuiLayer.h"
#include "InitializationGraph.h"
#include "Renderer.h"
#include "SystemManager.h"

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
	App() {};
	~App();
public:
	bool Init( HINSTANCE hInstance, int nCmdShow );
	void Run();
	void UpdateInputState();
	void UpdateCameraFromInput();
public:
	std::shared_ptr<InitializationGraph> BuildInitializationGraph( Renderer& renderer );
	std::shared_ptr<ExecutionGraph> BuildSimulationGraph( Renderer& renderer );
	std::shared_ptr<ExecutionGraph> BuildRenderGraph( Renderer& renderer, ImGuiLayer& imguiLayer );
private:
	InputState inputState;
private:
	std::unique_ptr<SystemManager> systemManager;
};