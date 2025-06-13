#pragma once
#include "GraphPass.h"

class SystemManager;

class AssetInitializationPass :
	public GraphPass
{
public:
	AssetInitializationPass() = default;
	~AssetInitializationPass() = default;
public:
	void Init( SystemManager& systemManager ) override;
	void Execute( SystemManager& systemManager ) override;
};

