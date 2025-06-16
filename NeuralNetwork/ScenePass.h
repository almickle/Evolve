#pragma once
#include "GraphPass.h"
#include "SystemManager.h"
#include "Types.h"

class ScenePass :
	public GraphPass
{
public:
	ScenePass()
		: GraphPass( "ScenePass" )
	{
	}
public:
	void Execute( SystemManager& systemManager, const AssetID& sceneID ) override;
};

