#pragma once
#include <algorithm>
#include <imgui.h>
#include <imgui_node_editor.h>
#include <memory>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>
#include "NodeLibrary.h"
#include "NodeTypes.h"
#include "Types.h"
#include "UiNode.h"
#include "UiNodeGraph.h"

using namespace ax;

void UiNodeGraph::Render()
{
	for( auto& node : nodes )
	{
		node->Render();
	}
	for( auto& link : links )
	{
		NodeEditor::Link( link->id, link->fromPin->GetID(), link->toPin->GetID() );
	}
}

int UiNodeGraph::AddNode( int& uniqueId, const NodeTypes& nodeType, NodeLibrary* nodeLibrary )
{
	auto node = std::make_unique<UiNode>( uniqueId, nodeType, nodeLibrary );
	nodes.push_back( std::move( node ) );
	return nodes.back()->GetID();
}

int UiNodeGraph::AddLink( int& uniqueId, int fromPin, int toPin )
{
	auto link = std::make_unique<UiNodeLink>();
	link->id = ++uniqueId;
	link->fromPin = FindPin( fromPin );
	link->toPin = FindPin( toPin );
	links.push_back( std::move( link ) );
	return links.back()->id;
}

int UiNodeGraph::AddLink( int& uniqueId, const int& fromNode, const int& fromPin, const int& toNode, const int& toPin )
{
	auto link = std::make_unique<UiNodeLink>();
	link->id = ++uniqueId;
	link->fromPin = nodes[fromNode]->GetOutputPins()[fromPin].get();
	link->toPin = nodes[toNode]->GetInputPins()[toPin].get();
	links.push_back( std::move( link ) );
	return links.back()->id;
}

UiNodePin* UiNodeGraph::FindPin( const int& id )
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

UiNode* UiNodeGraph::GetNode( const int& id )
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

UiNodeLink* UiNodeGraph::GetLink( const int& id )
{
	for( auto& link : links )
	{
		if( link->id == id )
		{
			return link.get();
		}
	}
	return nullptr;
}

NodeConnection UiNodeGraph::GetConnectionDetails( const int& id )
{
	NodeConnection connection{};

	auto link = GetLink( id );
	if( !link )
		return connection;

	// Find output node and pin index
	for( size_t nodeIdx = 0; nodeIdx < nodes.size(); ++nodeIdx )
	{
		auto& node = nodes[nodeIdx];
		const auto& inputs = node->GetInputPins();
		for( size_t pinIdx = 0; pinIdx < inputs.size(); ++pinIdx )
		{
			if( inputs[pinIdx].get() == link->fromPin )
			{
				connection.toNode = static_cast<uint>( nodeIdx );
				connection.toSlot = static_cast<uint>( pinIdx );
			}
		}
		const auto& outputs = node->GetOutputPins();
		for( size_t pinIdx = 0; pinIdx < outputs.size(); ++pinIdx )
		{
			if( outputs[pinIdx].get() == link->toPin )
			{
				connection.fromNode = static_cast<uint>( nodeIdx );
				connection.fromSlot = static_cast<uint>( pinIdx );
			}
		}
	}
	return connection;
}

void UiNodeGraph::Format()
{
	// --- Topological Sort ---
	size_t nodeCount = nodes.size();
	std::vector<std::vector<size_t>> adj( nodeCount ); // node index -> dependent node indices
	std::vector<size_t> inDegree( nodeCount, 0 );

	// Map pin pointer to node index for fast lookup
	std::unordered_map<UiNodePin*, size_t> pinToNode;
	for( size_t i = 0; i < nodeCount; ++i ) {
		for( const auto& pin : nodes[i]->GetOutputPins() )
			pinToNode[pin.get()] = i;
		for( const auto& pin : nodes[i]->GetInputPins() )
			pinToNode[pin.get()] = i;
	}

	// Build graph: for each link, fromPin -> toPin
	for( const auto& link : links ) {
		auto fromIt = pinToNode.find( link->fromPin );
		auto toIt = pinToNode.find( link->toPin );
		if( fromIt != pinToNode.end() && toIt != pinToNode.end() ) {
			adj[fromIt->second].push_back( toIt->second );
			inDegree[toIt->second]++;
		}
	}

	// Kahn's algorithm for topological sort
	std::vector<size_t> sorted;
	std::queue<size_t> q;
	for( size_t i = 0; i < nodeCount; ++i )
		if( inDegree[i] == 0 )
			q.push( i );

	while( !q.empty() ) {
		size_t idx = q.front(); q.pop();
		sorted.push_back( idx );
		for( size_t dep : adj[idx] ) {
			if( --inDegree[dep] == 0 )
				q.push( dep );
		}
	}

	// If not all nodes are sorted, there is a cycle; fallback to original order
	if( sorted.size() != nodeCount ) {
		sorted.clear();
		for( size_t i = 0; i < nodeCount; ++i )
			sorted.push_back( i );
	}

	// --- Layered Positioning ---
	// Assign each node a layer (distance from input nodes)
	std::vector<int> layer( nodeCount, 0 );
	for( size_t idx : sorted ) {
		for( size_t dep : adj[idx] ) {
			layer[dep] = std::max( layer[dep], layer[idx] + 1 );
		}
	}

	// Group nodes by layer
	std::unordered_map<int, std::vector<size_t>> nodesByLayer;
	int maxLayer = 0;
	for( size_t i = 0; i < nodeCount; ++i ) {
		nodesByLayer[layer[i]].push_back( i );
		if( layer[i] > maxLayer ) maxLayer = layer[i];
	}

	// --- Centered Vertical Distribution with Tuned Spacing ---
	const float xSpacing = 300.0f;
	const float baseYSpacing = 100.0f;
	const float pinSpacingFactor = 10.0f;
	const float centerY = 0.0f;

	// Find the maximum pin count among all nodes
	size_t maxPinCount = 1;
	for( const auto& node : nodes ) {
		size_t pinCount = std::max( node->GetInputPins().size(), node->GetOutputPins().size() );
		if( pinCount > maxPinCount )
			maxPinCount = pinCount;
	}

	float ySpacing = baseYSpacing + pinSpacingFactor * static_cast<float>(maxPinCount);

	// --- Sort nodes in each column by the index of the input pin they are connected to (lower index = higher) ---
	for( int l = 1; l <= maxLayer; ++l ) {
		auto& layerNodes = nodesByLayer[l];
		// For each node in this layer, find the minimum input pin index it is connected to from the previous layer
		std::vector<std::pair<size_t, int>> nodeAndPinIndex;
		for( size_t idx : layerNodes ) {
			int minPinIndex = INT_MAX;
			const auto& inputPins = nodes[idx]->GetInputPins();
			for( size_t pinIdx = 0; pinIdx < inputPins.size(); ++pinIdx ) {
				UiNodePin* pin = inputPins[pinIdx].get();
				for( const auto& link : links ) {
					if( link->toPin == pin ) {
						size_t fromNodeIdx = pinToNode[link->fromPin];
						if( layer[fromNodeIdx] == l - 1 ) {
							minPinIndex = static_cast<int>( pinIdx );
							break;
						}
					}
				}
				if( minPinIndex != INT_MAX ) break;
			}
			nodeAndPinIndex.emplace_back( idx, minPinIndex );
		}
		// Sort by pin index (lower index = higher up; nodes with no connection to previous layer go last)
		std::sort( nodeAndPinIndex.begin(), nodeAndPinIndex.end(),
				   []( const std::pair<size_t, int>& a, const std::pair<size_t, int>& b ) {
					   return a.second < b.second;
				   } );
		// Overwrite layerNodes with sorted order
		for( size_t i = 0; i < layerNodes.size(); ++i ) {
			layerNodes[i] = nodeAndPinIndex[i].first;
		}
	}

	// Now position nodes
	for( int l = 0; l <= maxLayer; ++l ) {
		const auto& layerNodes = nodesByLayer[l];
		size_t count = layerNodes.size();
		if( count == 0 ) continue;

		float totalHeight = (count - 1) * ySpacing;
		float startY = centerY - totalHeight / 2.0f;

		for( size_t i = 0; i < count; ++i ) {
			size_t idx = layerNodes[i];
			ImVec2 pos( l * xSpacing, startY + i * ySpacing );
			NodeEditor::SetNodePosition( nodes[idx]->GetID(), pos );
		}
	}
}