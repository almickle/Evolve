#pragma once
#include <memory>
#include <utility>
#include <vector>
#include "NodeLibrary.h"
#include "NodeTypes.h"
#include "UiNode.h"

struct UiNodeLink
{
	int id = 0;
	UiNodePin* fromPin = nullptr;
	UiNodePin* toPin = nullptr;
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
	void Render();
public:
	int AddNode( int& uniqueId, const NodeTypes& nodeType, NodeLibrary* nodeLibrary );
	int AddLink( int& uniqueId, int fromPin, int toPin );
	int AddLink( int& uniqueId, const int& fromNode, const int& fromPin, const int& toNode, const int& toPin );
public:
	UiNodePin* FindPin( const int& id );
	UiNode* GetNode( const int& id );
	UiNodeLink* GetLink( const int& id );
	NodeConnection GetConnectionDetails( const int& id );
	void Format();
public:
	std::vector<std::unique_ptr<UiNode>>& GetNodes() { return nodes; };
	std::vector<std::unique_ptr<UiNodeLink>>& GetLinks() { return links; };
private:
	std::vector<std::unique_ptr<UiNode>> nodes;
	std::vector<std::unique_ptr<UiNodeLink>> links;
};