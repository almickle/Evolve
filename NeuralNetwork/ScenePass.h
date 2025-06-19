#pragma once
#include "AssetManager.h"
#include "DescriptorHeapManager.h"
#include "GpuResourceManager.h"
#include "GraphPass.h"
#include "Renderer.h"
#include "SystemManager.h"
#include "Types.h"

class ScenePass :
	public GraphPass
{
public:
	ScenePass()
		: GraphPass( "ScenePass" )
	{
	}
public:
	void Execute( SystemManager& systemManager, const AssetID& sceneID ) override;
private:
	void BeginFrame( Renderer* renderer, DescriptorHeapManager* srvHeapManager );
	void EndFrame( Renderer* renderer );
	void DrawActors( AssetManager* assetManager, GpuResourceManager* resourceManager, Renderer* renderer, const AssetID& sceneID );
};

