#pragma once
#include <string>
#include <vector>
#include "Types.h"

class SubAsset {
public:
	SubAsset( SubAssetType subType = SubAssetType::Unknown, const std::string& name = "SubAsset" )
		: subType( subType ), name( name )
	{
	}
	virtual ~SubAsset() = default;
	const std::vector<ResourceID>& GetResourceIDs() const { return resourceIDs; }
	void AddResource( const ResourceID& id ) { resourceIDs.push_back( id ); }
protected:
	SubAssetType subType;
	std::string name;
	std::vector<ResourceID> resourceIDs;
};