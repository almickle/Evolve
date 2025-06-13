#pragma once
#include "GraphPass.h"

class SystemManager;

class BeginFramePass :
	public GraphPass {
public:
	BeginFramePass()
		: GraphPass( "BeginFrame" )
	{
	}
public:
	void Init( SystemManager& systemManager ) override;
	void Execute( SystemManager& systemManager ) override;
};