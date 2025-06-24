#pragma once
#include <drawing.h>
#include <imgui_node_editor.h>
#include <memory>
#include <string>
#include <vector>
#include <widgets.h>
#include "imgui.h"
#include "NodeLibrary.h"
#include "NodeTypes.h"
#include "Types.h"

using namespace ax;

struct PinProfile
{
	ImColor color = ImColor( 1.0f, 1.0f, 1.0f, 1.0f );
	Drawing::IconType iconType;
};

class UiNodePin
{
public:
	UiNodePin( int& uniqueId, const std::string& name, const NodeEditor::PinKind& kind, const NodeDataType& dataType )
		: id( ++uniqueId ),
		name( name ),
		kind( kind ),
		dataType( dataType )
	{
	}
public:
	void Render( const ImColor& backgroundColor = ImColor( 1, 1, 1, 1 ) )
	{
		if( kind == NodeEditor::PinKind::Input )
		{
			NodeEditor::BeginPin( id, kind );
			Widgets::Icon( ImVec2( size, size ), GetPinProfile().iconType, connected, GetPinProfile().color, backgroundColor );
			ImGui::SameLine();
			ImGui::Text( name.c_str() );
			NodeEditor::EndPin();
		}
		else if( kind == NodeEditor::PinKind::Output )
		{
			NodeEditor::BeginPin( id, kind );
			ImGui::Text( name.c_str() );
			ImGui::SameLine();
			Widgets::Icon( ImVec2( size, size ), GetPinProfile().iconType, connected, GetPinProfile().color, backgroundColor );
			NodeEditor::EndPin();
		}
	}
public:
	std::string GetName() const { return name; }
	const uint& GetID() const { return id; }
private:
	PinProfile GetPinProfile()
	{
		PinProfile profile{}; // white circle by default
		profile.iconType = Drawing::IconType::Circle;

		switch( dataType )
		{
			case NodeDataType::Float:
				profile.color = ImColor( 0.20f, 0.80f, 0.20f, 1.0f );
				break;
			case NodeDataType::Int:
				profile.color = ImColor( 0.20f, 0.60f, 1.00f, 1.0f );
				break;
			case NodeDataType::Uint:
				profile.color = ImColor( 0.20f, 0.60f, 1.00f, 1.0f );
				break;
			case NodeDataType::Color:
				profile.color = ImColor( 1.00f, 0.90f, 0.10f, 1.0f );
				break;
			case NodeDataType::Vector:
				profile.color = ImColor( 0.60f, 0.20f, 0.90f, 1.0f );
				break;
			case NodeDataType::String:
				profile.color = ImColor( 1.00f, 0.55f, 0.00f, 1.0f );
				break;
			case NodeDataType::Bool:
				profile.color = ImColor( 1.00f, 0.20f, 0.20f, 1.0f );
				break;
		}
		return profile;
	}
private:
	std::string name;
	uint id;
	NodeEditor::PinKind kind;
	NodeDataType dataType;
	float size = 16.0f;
	bool connected = false;
private:
};


class UiNode
{
public:
	UiNode( int& uniqueId, const NodeTypes& nodeType, NodeLibrary* nodeLibrary )
		: id( ++uniqueId ), nodeType( nodeType ), nodeLibrary( nodeLibrary )
	{
		auto* templateNode = nodeLibrary->GetNode( nodeType );
		name = templateNode->GetName();
		for( const auto& pin : templateNode->GetInputs() )
		{
			inputPins.push_back( std::make_unique<UiNodePin>( uniqueId, pin.GetName(), NodeEditor::PinKind::Input, pin.GetDataType() ) );
		}
		for( const auto& pin : templateNode->GetOutputs() )
		{
			outputPins.push_back( std::make_unique<UiNodePin>( uniqueId, pin.GetName(), NodeEditor::PinKind::Output, pin.GetDataType() ) );
		}
	};
	~UiNode() = default;
public:
	void Render()
	{
		NodeEditor::BeginNode( id );
		ImGui::Text( name.c_str() );
		ImGui::Dummy( ImVec2( 0, 2 ) );

		ImGui::BeginGroup(); // left side (inputs)
		for( auto& pin : inputPins )
		{
			pin->Render();
		}
		ImGui::EndGroup();
		ImGui::SameLine();

		ImGui::BeginGroup(); // right side (outputs)
		for( auto& pin : outputPins )
		{
			ImGui::Dummy( ImVec2( 10, 0 ) );
			ImGui::SameLine();
			pin->Render();
		}
		ImGui::EndGroup();

		NodeEditor::EndNode();
	}
public:
	UiNodePin* FindPin( int id )
	{
		for( auto& pin : inputPins ) {
			if( pin->GetID() == id ) {
				return pin.get();
			}
		}
		for( auto& pin : outputPins ) {
			if( pin->GetID() == id ) {
				return pin.get();
			}
		}
		return nullptr;
	};
public:
	std::string GetName() const { return name; }
	const uint& GetID() const { return id; }
	const std::vector<std::unique_ptr<UiNodePin>>& GetInputPins() const { return inputPins; }
	const std::vector<std::unique_ptr<UiNodePin>>& GetOutputPins() const { return outputPins; }
private:
	int id;
	std::string name;
	std::vector<std::unique_ptr<UiNodePin>> inputPins;
	std::vector<std::unique_ptr<UiNodePin>> outputPins;
	NodeTypes nodeType;
private:
	NodeLibrary* nodeLibrary;
};