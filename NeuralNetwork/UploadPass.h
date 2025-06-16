#pragma once
#include "GraphPass.h"
#include "SystemManager.h"
#include "Types.h"

class UploadPass
	: public GraphPass {
public:
	UploadPass()
		: GraphPass( "UploadPass" )
	{
	}
public:
	void Init( SystemManager& systemManager ) override;
	void Execute( SystemManager& systemManager, const AssetID& sceneID ) override;
};