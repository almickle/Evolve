#include <cstdarg>
#include <imgui.h>
#include <imgui_node_editor.h>
#include <memory>
#include <string>
#include <utility>
#include "MeshShader.h"
#include "NodeTypes.h"
#include "PixelShader.h"
#include "ShaderEditor.h"
#include "ShaderGraph.h"
#include "ShaderInstance.h"
#include "TextureAsset.h"
#include "Types.h"
#include "UiComponents.h"
#include "UiNodeGraph.h"

using namespace ax;

void ShaderEditor::Render()
{
	ImGui::Begin( "Shader Editor", nullptr );
	if( ImGui::BeginTabBar( "Shader Tabs" ) )
	{
		for( uint i = 0; i < shaderGraphs.size(); i++ )
		{
			auto* shaderGraph = shaderGraphs[i];
			if( ImGui::BeginTabItem( shaderGraph->GetName().c_str() ) )
			{
				if( activeGraph != i ) activeGraph = i;

				RenderSidePanel();
				ImGui::SameLine();
				RenderNodeEditor();
				ImGui::EndTabItem();
			}
		}

		ImGui::SameLine();
		if( ImGui::TabItemButton( "+" ) )
		{
			ImGui::OpenPopup( "AddShaderPopup" );
		}
		if( ImGui::BeginPopup( "AddShaderPopup" ) ) {
			if( ImGui::Selectable( "Mesh Shader" ) ) {
				CreateNewShader( ShaderType::Mesh );
			}
			if( ImGui::Selectable( "Pixel Shader" ) ) {
				CreateNewShader( ShaderType::Pixel );
			}
			ImGui::Dummy( ImVec2( 0, 2 ) );
			ImGui::Separator();
			ImGui::Dummy( ImVec2( 0, 2 ) );
			if( ImGui::BeginMenu( "Load Existing Shader" ) ) {
				for( const auto& shader : assetManager->GetAllShaders() )
				{
					if( ImGui::Selectable( shader->GetName().c_str() ) )
					{
						BindShaderGraph( shader->GetAssetID() );
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}

		ImGui::EndTabBar();
	}
	ImGui::End();
}

void ShaderEditor::RenderNodeEditor()
{
	ImGui::BeginChild( "NodeEditorRegion", ImGui::GetContentRegionAvail(), false );
	NodeEditor::SetCurrentEditor( editor );
	NodeEditor::Begin( "Shader Editor" );
	GetActiveNodeGraph()->Render();
	if( shouldFormat )
	{
		Format();
		shouldFormat = false;
	}
	if( NodeEditor::BeginCreate( ImColor( 255, 255, 255 ), 2.0f ) )
	{
		NodeEditor::PinId startPinId = 0, endPinId = 0;
		if( NodeEditor::QueryNewLink( &startPinId, &endPinId ) )
		{
			auto newLinkPin = endPinId;
			for( const auto& node : GetActiveNodeGraph()->GetNodes() )
			{
				for( const auto& pin : node->GetInputPins() )
				{
					if( pin->GetID() != endPinId.Get() ) continue;
					newLinkPin = pin->GetID();
					break;
				}
				for( const auto& pin : node->GetOutputPins() )
				{
					if( pin->GetID() != endPinId.Get() ) continue;
					newLinkPin = pin->GetID();
					break;
				}
			}

			if( NodeEditor::AcceptNewItem() )
			{
				AddLink( startPinId.Get(), newLinkPin.Get() );
				NodeEditor::Suspend();
				NodeEditor::Resume();
			}
		}
	}
	NodeEditor::EndCreate();
	NodeEditor::End();
	NodeEditor::SetCurrentEditor( nullptr );
	ImGui::EndChild();
}

void ShaderEditor::RenderSidePanel()
{
	ImGui::BeginChild( "SidePanel", ImVec2( panelWidth, 0 ), true );

	// Calculate split height
	float fullHeight = ImGui::GetContentRegionAvail().y;
	float topHeight = fullHeight * 0.5f; // 50% top, 50% bottom — tweak as needed

	// --- TOP PANEL: Controls ---
	ImGui::BeginChild( "TopPanel", ImVec2( 0, topHeight ), true );

	if( ImGui::Button( "Compile" ) ) {
		CompileShader();
	}
	ImGui::SameLine();
	if( ImGui::Button( "Save" ) ) {
		SaveShader();
	}
	ImGui::SameLine();
	if( ImGui::Button( "Format" ) ) {
		shouldFormat = true;
	}

	if( ImGui::BeginCombo( "##AddNodeCombo", "Add Node", ImGuiComboFlags_NoArrowButton ) ) {
		for( const auto* node : nodeLibrary->GetNodes() ) {
			if( ImGui::Selectable( node->GetName().c_str() ) ) {
				AddNode( node->GetType() );
			}
		}
		ImGui::EndCombo();
	}

	ImGui::EndChild(); // End TopPanel

	// --- BOTTOM PANEL: Shader Instances ---
	ImGui::BeginChild( "BottomPanel", ImVec2( 0, 0 ), true ); // remaining space

	if( ImGui::BeginTabBar( "InstanceTabs" ) ) {
		for( ShaderInstance* instance : assetManager->GetShaderInstances( GetActiveShader()->GetAssetID() ) ) {
			if( ImGui::BeginTabItem( instance->GetName().c_str() ) ) {
				// Texture bindings
				int i = 0;
				if( instance->GetSrvBindings().size() > 0 )
					ImGui::Text( "Textures:" );
				for( auto& srvBinding : instance->GetSrvBindings() ) {
					ImGui::Text( "Slot %d:", i++ );
					ImGui::SameLine();
					if( ImGui::BeginCombo( ("##SrvBindingCombo" + std::to_string( reinterpret_cast<uintptr_t>(&srvBinding) )).c_str(), assetManager->GetAsset( srvBinding.data )->GetName().c_str() ) )
					{
						for( const auto& texture : assetManager->GetAllTextures() ) {
							if( ImGui::Selectable( texture->GetName().c_str() ) ) {
								srvBinding.data = texture->GetAssetID();
								instance->SetIsDirty( true );
							}
						}
						ImGui::EndCombo();
					}
				}

				// Vector bindings
				i = 0;
				if( instance->GetVectorBindings().size() > 0 )
					ImGui::Text( "Vectors:" );
				for( auto& vectorBinding : instance->GetVectorBindings() ) {
					ImGui::Text( "Slot %d:", i++ );
					ImGui::SameLine();
					if( UIComponents::VectorPicker( vectorBinding.data.x, vectorBinding.data.y, vectorBinding.data.z, vectorBinding.data.w ) )
					{
						instance->SetIsDirty( true );
					}
				}

				// Scalar bindings
				i = 0;
				if( instance->GetScalarBindings().size() > 0 )
					ImGui::Text( "Scalars:" );
				for( auto& scalarBinding : instance->GetScalarBindings() ) {
					ImGui::Text( "Slot %d:", i++ );
					ImGui::SameLine();
					if( ImGui::DragFloat( ("##" + std::to_string( i++ )).c_str(), &scalarBinding.data ) )
					{
						instance->SetIsDirty( true );
					}
				}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	ImGui::EndChild(); // End BottomPanel
	ImGui::EndChild(); // End SidePanel
}

void ShaderEditor::AddLink( const NodeEditor::PinId& startPinId, const NodeEditor::PinId& newLinkPin )
{
	auto id = GetActiveNodeGraph()->AddLink( uniqueId, startPinId.Get(), newLinkPin.Get() );
	auto edge = GetActiveNodeGraph()->GetConnectionDetails( id );
	GetActiveShader()->AddEdge( edge );
}

void ShaderEditor::AddNode( NodeTypes type )
{
	GetActiveNodeGraph()->AddNode( uniqueId, type, nodeLibrary );
	GetActiveShader()->AddNode( type );
}

void ShaderEditor::SaveShader()
{
	assetManager->SaveAsset( GetActiveShader()->GetAssetID() );
}

void ShaderEditor::CompileShader()
{
	GetActiveShader()->Compile( *renderer );
}

void ShaderEditor::Format()
{
	GetActiveNodeGraph()->Format();
}

void ShaderEditor::BindShaderGraph( AssetID shaderGraph )
{
	auto* shaderGraphPtr = assetManager->GetAsset<ShaderGraph>( shaderGraph );
	shaderGraphs.push_back( shaderGraphPtr );
	auto graph = std::make_unique<UiNodeGraph>();
	for( const auto& nodeType : shaderGraphPtr->GetNodes() )
	{
		graph->AddNode( uniqueId, nodeType, nodeLibrary );
	}
	for( const auto& edge : shaderGraphPtr->GetEdges() )
	{
		graph->AddLink( uniqueId, edge.fromNode, edge.fromSlot, edge.toNode, edge.toSlot );
	}
	nodeGraphs.push_back( std::move( graph ) );
}

void ShaderEditor::CreateNewShader( const ShaderType& shaderType )
{
	switch( shaderType )
	{
		case ShaderType::Mesh:
		{
			auto shader = std::make_unique<MeshShader>( *nodeLibrary, "New Mesh Shader" );
			auto id = assetManager->RegisterAsset( std::move( shader ) );
			auto* shaderPtr = assetManager->GetAsset<MeshShader>( id );
			shaderGraphs.push_back( shaderPtr );
			break;
		}
		case ShaderType::Pixel:
		{
			auto shader = std::make_unique<PixelShader>( *nodeLibrary, "New Pixel Shader" );
			auto id = assetManager->RegisterAsset( std::move( shader ) );
			auto* shaderPtr = assetManager->GetAsset<PixelShader>( id );
			shaderGraphs.push_back( shaderPtr );
			break;
		}
	}
	auto graph = std::make_unique<UiNodeGraph>();
	nodeGraphs.push_back( std::move( graph ) );
}