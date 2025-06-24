#pragma once
#include <imgui_node_editor.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "imgui.h"
#include "MeshShader.h"
#include "NodeLibrary.h"
#include "NodeTypes.h"
#include "ShaderGraph.h"
#include "Types.h"
#include "UiNode.h"
#include "UiNodeGraph.h"

using namespace ax;

class ShaderEditor
{
public:
	ShaderEditor( NodeLibrary* nodeLibrary )
		: nodeLibrary( nodeLibrary )
	{
		config.SettingsFile = "Simple.json";
		editor = NodeEditor::CreateEditor( &config );

		auto nodea = std::make_unique<UiNode>( uniqueId, NodeTypes::Multiply, nodeLibrary );
		auto nodeb = std::make_unique<UiNode>( uniqueId, NodeTypes::VectorBreak, nodeLibrary );

		std::vector<std::unique_ptr<UiNode>> nodes;
		nodes.push_back( std::move( nodea ) );
		nodes.push_back( std::move( nodeb ) );
		std::vector<std::unique_ptr<UiNodeLink>> links;

		auto graph = std::make_unique<UiNodeGraph>( std::move( nodes ), std::move( links ) );

		nodeGraphs.push_back( std::move( graph ) );
		shaderGraphs.push_back( new MeshShader( *nodeLibrary, "Default" ) );
	}
	~ShaderEditor()
	{
		NodeEditor::DestroyEditor( editor );
	}
public:
	void Render()
	{
		ImGui::Begin( "Shader Editor", nullptr );
		if( ImGui::BeginTabBar( "Shader Tabs" ) )
		{
			for( uint i = 0; i < shaderGraphs.size(); i++ )
			{
				auto* shaderGraph = shaderGraphs[i];
				if( ImGui::BeginTabItem( shaderGraph->GetName().c_str() ) )
				{
					if( activeNodeGraph != i ) activeNodeGraph = i;

					RenderPropertyPanel();
					ImGui::SameLine();
					RenderNodeEditor();
					ImGui::EndTabItem();
				}
			}

			ImGui::SameLine();
			if( ImGui::TabItemButton( "+" ) )
			{
				CreateNewShader();
			}
			// Debugging information
			ImGui::SameLine();
			ImGui::TabItemButton( std::to_string( debugValue ).c_str() );

			ImGui::EndTabBar();
		}
		ImGui::End();
	}
public:
	void BindShaderGraph( ShaderGraph* shaderGraph )
	{
		shaderGraphs.push_back( shaderGraph );
		auto graph = std::make_unique<UiNodeGraph>();
		nodeGraphs.push_back( std::move( graph ) );
	}
	void CreateNewShader()
	{
		shaderGraphs.push_back( new MeshShader( *nodeLibrary ) );
		auto graph = std::make_unique<UiNodeGraph>();
		nodeGraphs.push_back( std::move( graph ) );
	}
	void ConvertToShader( ShaderGraph* shaderGraph, UiNodeGraph& uiGraph )
	{
		for( const auto& node : uiGraph.GetNodes() )
		{
			//shaderGraph->AddNode();
		}
	}
	void RenderPropertyPanel()
	{
		ImGui::BeginChild( "SidePanel", ImVec2( panelWidth, 0 ), true );
		ImGui::Button( "Compile" );
		ImGui::SameLine();
		ImGui::Button( "Save" );
		ImGui::SameLine();
		if( ImGui::BeginCombo( "##AddNodeCombo", "Add Node", ImGuiComboFlags_NoArrowButton ) )
		{
			for( const auto* node : nodeLibrary->GetNodes() )
			{
				if( ImGui::Selectable( node->GetName().c_str() ) )
				{
					GetActiveNodeGraph()->AddNode( uniqueId, node->GetType(), nodeLibrary );
				}
			}
			ImGui::EndCombo();
		}
		ImGui::EndChild();
	}
	void RenderNodeEditor()
	{
		ImGui::BeginChild( "NodeEditorRegion", ImGui::GetContentRegionAvail(), false );
		NodeEditor::SetCurrentEditor( editor );
		NodeEditor::Begin( "Shader Editor" );
		GetActiveNodeGraph()->Render();
		if( NodeEditor::BeginCreate( ImColor( 255, 255, 255 ), 2.0f ) )
		{
			NodeEditor::PinId startPinId = 0, endPinId = 0;
			if( NodeEditor::QueryNewLink( &startPinId, &endPinId ) )
			{
				//debugValue = endPinId.Get();
				auto newLinkPin = endPinId;
				for( const auto& node : GetActiveNodeGraph()->GetNodes() )
				{
					for( const auto& pin : node->GetInputPins() )
					{
						if( pin->GetID() == endPinId.Get() )
						{
							newLinkPin = pin->GetID();
							break;
						}
					}
					for( const auto& pin : node->GetOutputPins() )
					{
						if( pin->GetID() == endPinId.Get() )
						{
							newLinkPin = pin->GetID();
							break;
						}
					}
				}
				/*if( newLinkPin )
					showLabel( "+ Create Node", ImColor( 32, 45, 32, 180 ) );*/

				if( NodeEditor::AcceptNewItem() )
				{
					GetActiveNodeGraph()->AddLink( uniqueId, startPinId.Get(), newLinkPin.Get() );
					//createNewNode = true;
					//newNodeLinkPin = FindPin( pinId );
					//newLinkPin = nullptr;
					NodeEditor::Suspend();
					ImGui::OpenPopup( "Create New Node" );
					NodeEditor::Resume();
					debugValue = GetActiveNodeGraph()->GetLinks().size();
				}
			}
		}
		NodeEditor::EndCreate();
		NodeEditor::End();
		NodeEditor::SetCurrentEditor( nullptr );
		ImGui::EndChild();
	}
public:
	const NodeEditor::Config& GetConfig() const { return config; }
	NodeEditor::EditorContext* GetEditorContext() const { return editor; }
	UiNodeGraph* GetActiveNodeGraph() { return nodeGraphs[activeNodeGraph].get(); }
private:
	uint activeNodeGraph = 0;
	float panelWidth = 400.0f;
	int debugValue = 0;
private:
	NodeEditor::EditorContext* editor = nullptr;
	NodeEditor::Config config;
	std::vector<std::unique_ptr<UiNodeGraph>> nodeGraphs;
	std::vector<ShaderGraph*> shaderGraphs;
	int uniqueId = 1;
private:
	NodeLibrary* nodeLibrary;
};

