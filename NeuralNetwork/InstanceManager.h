#pragma once
#include <algorithm>
#include <unordered_map>
#include <vector>
#include "Actor.h"
#include "SceneData.h"
#include "Types.h"

class InstanceManager {
public:
	InstanceManager() = default;
	~InstanceManager() = default;
public:
	std::vector<Actor*> GetInstances( AssetID assetID )
	{
		auto it = instanceHeap.find( assetID );
		if( it != instanceHeap.end() ) {
			return it->second;
		}
		return {};
	}
	uint GetInstanceCount( AssetID assetID )
	{
		auto it = instanceHeap.find( assetID );
		if( it != instanceHeap.end() ) {
			return static_cast<uint>(it->second.size());
		}
		return 0;
	}
	std::vector<ActorTransformData> GetInstanceData( AssetID assetID )
	{
		auto it = instanceHeap.find( assetID );
		if( it != instanceHeap.end() ) {
			std::vector<ActorTransformData> instanceData;
			instanceData.reserve( it->second.size() );
			for( Actor* actor : it->second ) {
				instanceData.push_back( { actor->GetTransform() } );
			}
			return instanceData;
		}
		return {};
	}
	void AddInstance( AssetID assetID, Actor* actor )
	{
		instanceHeap[assetID].push_back( actor );
		if( std::find( uniqueAssets.begin(), uniqueAssets.end(), assetID ) == uniqueAssets.end() ) {
			uniqueAssets.push_back( assetID );
		}
	}
	void AddInstances( AssetID assetID, std::vector<Actor*> actors )
	{
		instanceHeap[assetID].insert( instanceHeap[assetID].end(), actors.begin(), actors.end() );
		for( Actor* actor : actors ) {
			if( std::find( uniqueAssets.begin(), uniqueAssets.end(), assetID ) == uniqueAssets.end() ) {
				uniqueAssets.push_back( assetID );
			}
		}
	}
	std::vector<AssetID>& GetUniqueAssets() { return uniqueAssets; }
	uint GetHeapOffset( const AssetID& id )
	{
		uint offset = 0;
		for( const auto& asset : uniqueAssets )
		{
			if( asset == id )
				break;
			offset += static_cast<uint>(instanceHeap[asset].size());
		}
		return offset;
	}
private:
	std::unordered_map<AssetID, std::vector<Actor*>> instanceHeap;
	std::vector<AssetID> uniqueAssets;
};