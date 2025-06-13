#pragma once
#include <d3d12.h>
#include "SystemManager.h"

class DescriptorHeapManager;
class Renderer;
class Window;

class ImGuiLayer {
public:
	ImGuiLayer( SystemManager& systemManager )
		: srvHeapManager( systemManager.GetSrvHeapManager() ),
		renderer( systemManager.GetRenderer() ),
		window( systemManager.GetWindow() )
	{
	}
	~ImGuiLayer();
public:
	void Init();
	void BeginFrame();
	void RenderUI();
	void EndFrame( ID3D12GraphicsCommandList* commandList );
private:
	int imguiFontSrvIndex = -1;
private:
	DescriptorHeapManager* srvHeapManager;
	Renderer* renderer;
	Window* window;
};