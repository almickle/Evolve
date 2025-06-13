#pragma once
#include "ExecutionGraph.h"

class SystemManager;

class InitializationGraph :
	public ExecutionGraph
{
public:
	InitializationGraph() = default;
public:
	void ExecuteSync( SystemManager& systemManager ) override;
	void ExecuteAsync( SystemManager& systemManager ) override;
};

