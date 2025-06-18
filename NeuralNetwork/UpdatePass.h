#pragma once
#include "GraphPass.h"
#include "SystemManager.h"
#include "Types.h"

class UpdatePass :
	public GraphPass
{
public:
	UpdatePass()
		: GraphPass( "UpdatePass" )
	{
	}
public:
	void Execute( SystemManager& systemManager, const AssetID& sceneID ) override;
};

