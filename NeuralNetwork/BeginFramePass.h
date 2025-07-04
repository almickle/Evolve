#pragma once
#include "GraphPass.h"
#include "Types.h"

class SystemManager;

class BeginFramePass :
	public GraphPass {
public:
	BeginFramePass()
		: GraphPass( "BeginFrame" )
	{
	}
public:
	void Execute( SystemManager& systemManager, const AssetID& sceneID ) override;
};