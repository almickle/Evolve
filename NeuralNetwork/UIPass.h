#pragma once
#include "GraphPass.h"
#include "SystemManager.h"
#include "Types.h"

class Renderer;
class ImGuiLayer;

class UIPass : public GraphPass {
public:
	UIPass()
		: GraphPass( "ScenePass" )
	{
	}
public:
	void Execute( SystemManager& systemManager, const AssetID& sceneID ) override;
};