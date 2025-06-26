#include "AssetManager.h"
#include "ConstantBuffer.h"
#include "GpuResourceManager.h"
#include "Scene.h"
#include "SceneData.h"
#include "SystemManager.h"
#include "Types.h"
#include "UpdatePass.h"

void UpdatePass::Execute( SystemManager& systemManager, const AssetID& sceneID )
{
	auto* renderer = systemManager.GetRenderer();
	auto* resourceManager = systemManager.GetResourceManager();
	auto* assetManager = systemManager.GetAssetManager();
	auto* srvHeapManager = systemManager.GetSrvHeapManager();
	auto* scene = static_cast<Scene*>(assetManager->GetAsset( sceneID ));
	auto* cb = static_cast<ConstantBuffer*>(resourceManager->GetResource( scene->GetSceneBuffer() ));

	auto frameIndex = renderer->GetCurrentFrameIndex();
	auto* allocator = commandAllocators[frameIndex].Get();
	auto* commandList = commandLists[frameIndex].Get();

	allocator->Reset();
	commandList->Reset( allocator, nullptr );

	// Initialize scene constant buffer
	CameraData camData{
		.view{scene->GetActiveCamera()->GetViewMatrix()},
		.proj{scene->GetActiveCamera()->GetProjectionMatrix()},
		.position{scene->GetActiveCamera()->GetPosition()}
	};
	LightData lightData[8]{};
	for( size_t i = 0; i < scene->GetLightCount(); ++i ) {
		lightData[i].type = scene->GetLights()[i]->GetType();
		lightData[i].position = scene->GetLights()[i]->GetPosition();
		lightData[i].intensity = scene->GetLights()[i]->GetIntensity();
		lightData[i].color = scene->GetLights()[i]->GetColor();
	}
	SceneData sceneData{
		.camera = camData,
		.lights = {lightData[0], lightData[1], lightData[2], lightData[3],
				   lightData[4], lightData[5], lightData[6], lightData[7]},
		.numLights = static_cast<uint>( scene->GetLightCount() )
	};

	cb->Update( &sceneData, sizeof( SceneData ) );

	for( auto* asset : assetManager->GetAllAssets() ) {
		if( asset->IsDirty() ) {
			asset->Update( assetManager, resourceManager, srvHeapManager );
			for( const auto& resourceId : asset->GetAllResourceIDs() )
			{
				resourceManager->GetResource( resourceId )->Upload( commandList );
			}
			asset->SetIsDirty( false );
		}
	}
	commandList->Close();
}