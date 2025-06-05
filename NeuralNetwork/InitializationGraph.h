#pragma once
#include "ExecutionGraph.h"

class Renderer;

class InitializationGraph :
	public ExecutionGraph
{
public:
	InitializationGraph() = default;
public:
	void ExecuteSync( Renderer& renderer ) override;
	void ExecuteAsync( Renderer& renderer ) override;
};

