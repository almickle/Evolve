#pragma once
#include <imgui_node_editor.h>
#include <memory>
#include <vector>
#include "AssetManager.h"
#include "NodeLibrary.h"
#include "NodeTypes.h"
#include "Renderer.h"
#include "ShaderGraph.h"
#include "Types.h"
#include "UiNodeGraph.h"

using namespace ax;

class ShaderEditor
{
public:
	ShaderEditor( NodeLibrary* nodeLibrary, AssetManager* assetManager, Renderer* renderer )
		: nodeLibrary( nodeLibrary ),
		assetManager( assetManager ),
		renderer( renderer )
	{
		config.SettingsFile = "NodeEditor.json";
		editor = NodeEditor::CreateEditor( &config );
	}
	~ShaderEditor()
	{
		NodeEditor::DestroyEditor( editor );
	}
public:
	const NodeEditor::Config& GetConfig() const { return config; }
	NodeEditor::EditorContext* GetEditorContext() const { return editor; }
	UiNodeGraph* GetActiveNodeGraph() { return nodeGraphs[activeGraph].get(); }
	ShaderGraph* GetActiveShader() { return shaderGraphs[activeGraph]; }
public:
	void Render();
private:
	void RenderNodeEditor();
	void RenderSidePanel();
private:
	void AddLink( const NodeEditor::PinId& startPinId, const NodeEditor::PinId& newLinkPin );
	void AddNode( NodeTypes type );
	void SaveShader();
	void CompileShader();
	void Format();
	void BindShaderGraph( AssetID shaderGraph );
	void CreateNewShader( const ShaderType& shaderType );
private:
	uint activeGraph = 0;
	float panelWidth = 400.0f;
	int debugValue = 0;
	bool shouldFormat = true;
private:
	NodeEditor::EditorContext* editor = nullptr;
	NodeEditor::Config config;
	std::vector<std::unique_ptr<UiNodeGraph>> nodeGraphs;
	std::vector<ShaderGraph*> shaderGraphs;
	int uniqueId = 1;
private:
	NodeLibrary* nodeLibrary;
	AssetManager* assetManager;
	Renderer* renderer;
};

