#include <combaseapi.h>
#include <memory>
#include <objbase.h>
#include <sal.h>
#include <Windows.h>
#include "App.h"
#include "BeginFramePass.h"
#include "Camera.h"
#include "EndFramePass.h"
#include "ExecutionGraph.h"
#include "ImGuiLayer.h"
#include "InitializationGraph.h"
#include "Renderer.h"
#include "UIRenderPass.h"

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow )
{
	App app;
	if( !app.Init( hInstance, nCmdShow ) )
		return -1;

	app.Run();
	app.Shutdown();
	return 0;
}

bool App::Init( HINSTANCE hInstance, int nCmdShow )
{
	HRESULT hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
	if( FAILED( hr ) ) {
		MessageBox( nullptr, L"Failed to initialize COM library", L"Error", MB_OK | MB_ICONERROR );
		return false;
	}
	if( !window.Create( L"DX12 ImGui App", hInstance, nCmdShow ) )
		return false;
	if( !renderer.Init( window.GetHWND() ) )
		return false;
	imgui.Init( window.GetHWND(), renderer );

	auto simGraph = BuildSimulationGraph( renderer );
	auto renderGraph = BuildRenderGraph( renderer, imgui );
	auto initGraph = BuildInitializationGraph( renderer );

	// Center the mouse in the window
	RECT rect;
	GetClientRect( window.GetHWND(), &rect );
	POINT center{};
	center.x = (rect.right - rect.left) / 2;
	center.y = (rect.bottom - rect.top) / 2;
	ClientToScreen( window.GetHWND(), &center );
	SetCursorPos( center.x, center.y );

	// Initialize input state
	inputState.lastMouseX = center.x;
	inputState.lastMouseY = center.y;
	inputState.firstMouse = false;

	initGraph->ExecuteAsync( renderer );

	return true;
}

void App::Run()
{
	while( !window.ShouldClose() ) {
		window.PollEvents();
		//UpdateInputState();
		//UpdateCameraFromInput();

		renderer.GetSimulationGraph()->ExecuteAsync( renderer );
		renderer.GetRenderGraph()->ExecuteAsync( renderer );

		renderer.Present();
	}
}

std::shared_ptr<InitializationGraph> App::BuildInitializationGraph( Renderer& renderer )
{
	auto graph = std::make_shared<InitializationGraph>();

	for( const auto& pass : renderer.GetSimulationGraph()->GetPasses() ) {
		graph->AddPass( pass );
	}
	for( const auto& pass : renderer.GetRenderGraph()->GetPasses() ) {
		graph->AddPass( pass );
	}

	renderer.SetInitializationGraph( graph );
	return graph;
}

std::shared_ptr<ExecutionGraph> App::BuildSimulationGraph( Renderer& renderer )
{
	auto graph = std::make_shared<ExecutionGraph>();

	renderer.SetSimulationGraph( graph );
	return graph;
}

std::shared_ptr<ExecutionGraph> App::BuildRenderGraph( Renderer& renderer, ImGuiLayer& imguiLayer )
{
	auto graph = std::make_shared<ExecutionGraph>();

	auto beginPass = std::make_shared<BeginFramePass>();
	//auto uiPass = std::make_shared<UIRenderPass>( renderer, &imguiLayer );
	auto endPass = std::make_shared<EndFramePass>();

	//uiPass->AddDependency( beginPass );
	endPass->AddDependency( beginPass );

	graph->AddPass( beginPass );
	//graph->AddPass( uiPass );
	graph->AddPass( endPass );

	renderer.SetRenderGraph( graph );
	return graph;
}


void App::UpdateInputState()
{
	// Keyboard
	for( int i = 0; i < 256; ++i ) {
		inputState.keys[i] = (GetAsyncKeyState( i ) & 0x8000) != 0;
	}

	// Mouse
	POINT p;
	if( GetCursorPos( &p ) ) {
		ScreenToClient( window.GetHWND(), &p );
		if( inputState.firstMouse ) {
			inputState.lastMouseX = p.x;
			inputState.lastMouseY = p.y;
			inputState.firstMouse = false;
		}
		inputState.mouseDeltaX = p.x - inputState.lastMouseX;
		inputState.mouseDeltaY = p.y - inputState.lastMouseY;
		inputState.lastMouseX = p.x;
		inputState.lastMouseY = p.y;

		// Only recenter if window is focused
		if( GetForegroundWindow() == window.GetHWND() ) {
			RECT rect;
			GetClientRect( window.GetHWND(), &rect );
			POINT center;
			center.x = (rect.right - rect.left) / 2;
			center.y = (rect.bottom - rect.top) / 2;
			ClientToScreen( window.GetHWND(), &center );
			SetCursorPos( center.x, center.y );
			inputState.lastMouseX = center.x;
			inputState.lastMouseY = center.y;
		}
	}
}

void App::UpdateCameraFromInput()
{
	//auto camera = scene.GetActiveCamera();
	//if( !camera ) return;

	//float moveSpeed = 0.1f;
	//float rotSpeed = 0.005f;

	//if( inputState.keys['W'] ) camera->MoveForward( moveSpeed );
	//if( inputState.keys['S'] ) camera->MoveForward( -moveSpeed );
	//if( inputState.keys['A'] ) camera->MoveRight( -moveSpeed );
	//if( inputState.keys['D'] ) camera->MoveRight( moveSpeed );

	//if( !inputState.firstMouse ) {
	//	camera->Rotate( inputState.mouseDeltaX * rotSpeed, inputState.mouseDeltaY * rotSpeed );
	//}
	//inputState.mouseDeltaX = 0;
	//inputState.mouseDeltaY = 0;
}

void App::Shutdown()
{
	renderer.Shutdown();
	window.Destroy();
	CoUninitialize();
}