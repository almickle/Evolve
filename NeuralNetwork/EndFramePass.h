#pragma once
#include "GraphPass.h"

class SystemManager;

class EndFramePass :
	public GraphPass {
public:
	EndFramePass()
		: GraphPass( "EndFrame" )
	{
	}
	void Execute( SystemManager& systemManager, const AssetID& sceneID ) override;
};