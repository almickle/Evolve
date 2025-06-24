#pragma once
#include <imgui_node_editor.h>
#include <memory>
#include <utility>
#include <vector>
#include "NodeLibrary.h"
#include "NodeTypes.h"
#include "UiNode.h"

using namespace ax;

struct UiNodeLink
{
	int id = 0;
	UiNodePin* inputPin = nullptr;
	UiNodePin* outputPin = nullptr;
};

class UiNodeGraph
{
public:
	UiNodeGraph( std::vector<std::unique_ptr<UiNode>>&& nodes, std::vector<std::unique_ptr<UiNodeLink>>&& links )
		: nodes( std::move( nodes ) ), links( std::move( links ) )
	{
	};
	UiNodeGraph() = default;
	~UiNodeGraph() = default;
public:
	void Render()
	{
		for( auto& node : nodes )
		{
			node->Render();
		}
		for( auto& link : links )
		{
			NodeEditor::Link( link->id, link->inputPin->GetID(), link->outputPin->GetID() );
		}
	}
public:
	void AddNode( int& uniqueId, const NodeTypes& nodeType, NodeLibrary* nodeLibrary )
	{
		auto node = std::make_unique<UiNode>( uniqueId, nodeType, nodeLibrary );
		nodes.push_back( std::move( node ) );
	}
	void AddLink( int& uniqueId, int inputPin, int outputPin )
	{
		auto link = std::make_unique<UiNodeLink>();
		link->id = ++uniqueId;
		link->inputPin = FindPin( inputPin );
		link->outputPin = FindPin( outputPin );
		links.push_back( std::move( link ) );
	}
public:
	UiNodePin* FindPin( int id )
	{
		for( auto& node : nodes )
		{
			auto pin = node->FindPin( id );
			if( pin != nullptr )
			{
				return pin;
			}
		}
		return nullptr;
	}
	UiNode* GetNode( int id )
	{
		for( auto& node : nodes )
		{
			if( node->GetID() == id )
			{
				return node.get();
			}
		}
		return nullptr;
	}
	std::vector<std::unique_ptr<UiNode>>& GetNodes() { return nodes; };
	std::vector<std::unique_ptr<UiNodeLink>>& GetLinks() { return links; };
private:
	std::vector<std::unique_ptr<UiNode>> nodes;
	std::vector<std::unique_ptr<UiNodeLink>> links;
};