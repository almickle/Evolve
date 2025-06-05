#pragma once
#include "GraphPass.h"
#include "Renderer.h"

class BeginFramePass : public GraphPass {
public:
	BeginFramePass()
		: GraphPass( "BeginFrame" )
	{
	}
	void Init( Renderer& renderer ) override;
	void Execute( Renderer& renderer ) override;
	void Shutdown() override {}
};