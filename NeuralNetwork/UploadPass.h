#pragma once
#include "GraphPass.h"
#include "SystemManager.h"

class UploadPass : public GraphPass {
public:
	UploadPass() : GraphPass( "UploadPass" ) {}

	void Execute( SystemManager& systemManager ) override
	{
		// Let the UploadManager process all pending uploads
		if( systemManager.GetUploadManager() ) {
			//renderer.GetUploadManager()->Upload(renderer);
		}
	}
};