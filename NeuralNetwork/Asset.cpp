#include <vector>
#include "Asset.h"
#include "SubAsset.h"
#include "Types.h"


std::vector<ResourceID> Asset::GetAllResourceIDs() const
{
	std::vector<ResourceID> allIDs = resourceIDs;
	for( const auto& sub : subassets ) {
		const auto& subIDs = sub->GetResourceIDs();
		allIDs.insert( allIDs.end(), subIDs.begin(), subIDs.end() );
	}
	return allIDs;
}