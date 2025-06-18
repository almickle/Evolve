#include <d3d12.h>
#include "GpuResourceManager.h"
#include "SystemManager.h"
#include "Types.h"
#include "UploadManager.h"
#include "UploadPass.h"

void UploadPass::Init( SystemManager& systemManager )
{
	auto* resourceManager = systemManager.GetResourceManager();
	auto* uploadManager = systemManager.GetUploadManager();
	if( !uploadManager || !resourceManager )
		return;

	for( auto* resource : resourceManager->GetAllResources() )
	{
		UploadRequest request;
		// Lambda captures the resource pointer and calls its Upload method
		request.recordFunc = [resource]( ID3D12GraphicsCommandList* cmdList ) {
			resource->Upload( cmdList );
			};
		request.onComplete = [resource]() {
			resource->SetIsReady( true );
			};
		uploadManager->Enqueue( request );
	}

	uploadManager->Flush();
}

void UploadPass::Execute( SystemManager& systemManager, const AssetID& sceneID )
{
	auto* resourceManager = systemManager.GetResourceManager();
	auto* renderer = systemManager.GetRenderer();

	for( auto* resource : resourceManager->GetCurrentFrameResources() )
	{
		auto* cmdList = GetCurrentCommandList( renderer->GetCurrentFrameIndex() );
		resource->Upload( static_cast<ID3D12GraphicsCommandList*>(cmdList) );
	}
}