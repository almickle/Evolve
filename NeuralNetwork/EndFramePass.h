#pragma once
#include "GraphPass.h"
#include "Renderer.h"

class EndFramePass : public GraphPass {
public:
	EndFramePass()
		: GraphPass( "EndFrame" )
	{
	}
	void Init( Renderer& renderer ) override;
	void Execute( Renderer& renderer ) override;
	void Shutdown() override {};
};