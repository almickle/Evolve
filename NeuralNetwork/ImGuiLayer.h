#pragma once
#include <d3d12.h>
#include "NodeLibrary.h"
#include "ShaderEditor.h"
#include "SystemManager.h"

class DescriptorHeapManager;
class Renderer;
class Window;

enum class UiContextType
{
	None,
	ShaderEditor,
	AssetBrowser,
	DebugConsole
};

struct UiContext
{
	UiContextType currentContext = UiContextType::None;
};

class ImGuiLayer {
public:
	ImGuiLayer( SystemManager& systemManager )
		: srvHeapManager( systemManager.GetSrvHeapManager() ),
		renderer( systemManager.GetRenderer() ),
		window( systemManager.GetWindow() ),
		nodeLibrary( systemManager.GetNodeLibrary() ),
		shaderEditor( ShaderEditor( systemManager.GetNodeLibrary(), systemManager.GetAssetManager(), systemManager.GetRenderer() ) )
	{
	}
	~ImGuiLayer();
public:
	void Init();
	void BeginFrame();
	void RenderUI();
	void EndFrame( ID3D12GraphicsCommandList* commandList );
	bool UsingInput() const;
private:
	void SetContext( UiContextType context ) { uiContext.currentContext = context; }
private:
	int imguiFontSrvIndex = -1;
private:
	UiContext uiContext;
	ShaderEditor shaderEditor;
private:
	DescriptorHeapManager* srvHeapManager;
	Renderer* renderer;
	Window* window;
	NodeLibrary* nodeLibrary;
};