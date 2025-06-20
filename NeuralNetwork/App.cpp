#include <memory>
#include <sal.h>
#include <utility>
#include <Windows.h>
#include "App.h"
#include "AssetManager.h"
#include "ExecutionGraph.h"
#include "InputSystem.h"
#include "Renderer.h"
#include "Scene.h"
#include "ScenePass.h"
#include "SystemManager.h"
#include "UIPass.h"
#include "UpdatePass.h"
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
	auto* scene = static_cast<Scene*>(systemManager->GetAssetManager()->GetAsset( sceneID ));
	auto uploadPass = std::make_unique<UploadPass>();
	auto updatePass = std::make_unique<UpdatePass>();
	uploadPass->Init( *systemManager );
	while( !systemManager->GetWindow()->ShouldClose() ) {
		systemManager->GetWindow()->PollEvents();
		systemManager->GetRenderer()->WaitForCurrentFrame();
		systemManager->GetInputSystem()->Update( scene );
		updatePass->Execute( *systemManager, sceneID );
		uploadPass->Execute( *systemManager, sceneID );
		systemManager->GetRenderer()->GetRenderGraph()->ExecuteAsync( *systemManager, sceneID );
		systemManager->GetRenderer()->Present();
	}
}

void App::BuildRenderGraph()
{
	auto scenePass = std::make_unique<ScenePass>();
	auto uiPass = std::make_unique<UIPass>();
	uiPass->AddDependency( scenePass.get() );
	systemManager->GetRenderer()->GetRenderGraph()
		->AddPass( std::move( scenePass ) )
		->AddPass( std::move( uiPass ) );

	for( const auto& pass : systemManager->GetRenderer()->GetRenderGraph()->GetPasses() )
	{
		pass->Init( *systemManager );
	}
}