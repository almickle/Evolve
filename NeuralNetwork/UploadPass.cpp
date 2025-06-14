#include <d3d12.h>
#include "GpuResourceManager.h"
#include "SystemManager.h"
#include "UploadManager.h"
#include "UploadPass.h"

void UploadPass::Execute( SystemManager& systemManager )
{
	auto* resourceManager = systemManager.GetResourceManager();
	auto* uploadManager = systemManager.GetUploadManager();
	if( !uploadManager || !resourceManager )
		return;

	for( auto* resource : resourceManager->GetCurrentFrameResources() )
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

	uploadManager->FlushCurrentFrame();
}

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