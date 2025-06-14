#include <memory>
#include <sal.h>
#include <utility>
#include <Windows.h>
#include "App.h"
#include "BeginFramePass.h"
#include "EndFramePass.h"
#include "ExecutionGraph.h"
#include "Renderer.h"
#include "SystemManager.h"
#include "UploadManager.h"
#include "UploadPass.h"
#include "Window.h"

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow )
{
	App app;
	if( !app.Init( hInstance, nCmdShow ) )
		return -1;

	app.Run();
	return 0;
}

App::~App()
{
}

bool App::Init( HINSTANCE hInstance, int nCmdShow )
{
	systemManager->Init( { L"Hitari Engine", hInstance, nCmdShow } );

	BuildRenderGraph();

	return true;
}

void App::Run()
{
	auto uploadPass = std::make_unique<UploadPass>();
	uploadPass->Init( *systemManager );
	while( !systemManager->GetWindow()->ShouldClose() ) {
		systemManager->GetWindow()->PollEvents();
		//UpdateInputState();
		//UpdateCameraFromInput();
		systemManager->GetUploadManager()->WaitForCurrentFrame();
		systemManager->GetRenderer()->WaitForCurrentFrame();

		uploadPass->Execute( *systemManager );
		systemManager->GetRenderer()->GetRenderGraph()->ExecuteAsync( *systemManager );
		systemManager->GetRenderer()->Present();
	}
}

void App::BuildRenderGraph()
{
	auto beginPass = std::make_unique<BeginFramePass>();
	auto endPass = std::make_unique<EndFramePass>();
	endPass->AddDependency( beginPass.get() );

	systemManager->GetRenderer()->GetRenderGraph()
		->AddPass( std::move( beginPass ) )
		->AddPass( std::move( endPass ) );

	for( const auto& pass : systemManager->GetRenderer()->GetRenderGraph()->GetPasses() )
	{
		pass->Init( *systemManager );
	}
}