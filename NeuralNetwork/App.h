#pragma once
#include <memory>
#include <Windows.h>
#include "SystemManager.h"
#include "Types.h"

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
	App() :
		systemManager( std::make_unique<SystemManager>() )
	{
	};
	~App();
public:
	bool Init( HINSTANCE hInstance, int nCmdShow );
	void Run();

public:
	void BuildRenderGraph();
private:
	InputState inputState;
private:
	std::unique_ptr<SystemManager> systemManager;
	AssetID sceneID;
};