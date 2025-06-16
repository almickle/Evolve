#include "AssetManager.h"
#include "ConstantBuffer.h"
#include "GpuResourceManager.h"
#include "Model.h"
#include "Renderer.h"
#include "Scene.h"
#include "ScenePass.h"
#include "SystemManager.h"
#include "Types.h"

void ScenePass::Execute( SystemManager& systemManager, const AssetID& sceneID )
{
	auto* renderer = systemManager.GetRenderer();
	auto* assetManager = systemManager.GetAssetManager();
	auto* resourceManager = systemManager.GetResourceManager();

	renderer->UpdateCurrentFrameIndex();
	uint frameIndex = renderer->GetCurrentFrameIndex();

	auto* allocator = commandAllocators[frameIndex].Get();
	auto* commandList = commandLists[frameIndex].Get();

	allocator->Reset();
	commandList->Reset( allocator, nullptr );

	auto* scene = static_cast<Scene*>(assetManager->GetAsset( sceneID ));
	auto* sManager = scene->GetStaticInstanceManager();
	auto* dManager = scene->GetDynamicInstanceManager();

	auto* cb = static_cast<ConstantBuffer*>(resourceManager->GetResource( scene->GetSceneBuffer() ));
	renderer->BindSceneConstantBuffer( commandList, cb->GetResource()->GetGPUVirtualAddress() );

	for( auto& uniqueModel : sManager->GetUniqueAssets() )
	{
		uint instanceCount = sManager->GetInstanceCount( uniqueModel );
		uint heapOffset = sManager->GetHeapOffset( uniqueModel );

		auto* model = static_cast<Model*>(assetManager->GetAsset( uniqueModel ));
		auto psCbAddresses = model->GetMaterialConstantBufferAddresses( *assetManager, *resourceManager );

		auto& psoKeys = model->GetPsoKeys();
		auto vbViews = model->GetVertexBufferViews( *assetManager, *resourceManager );
		auto ibViews = model->GetIndexBufferViews( *assetManager, *resourceManager );

		renderer->RenderActorInstances( commandList, psoKeys, vbViews, ibViews, psCbAddresses, instanceCount, heapOffset, true );
	}
}