#pragma once
#include <memory>
#include <Windows.h>
#include "SystemManager.h"
#include "Types.h"

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
	std::unique_ptr<SystemManager> systemManager;
	AssetID sceneID = "Asset_9";
};