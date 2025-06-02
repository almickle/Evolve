#include "App.h"
#include "UIRenderPass.h"
#include "SceneRenderPass.h"
#include "RenderGraph.h"
#include "Camera.h"
#include <Windows.h>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	App app;
	if (!app.Init(hInstance, nCmdShow))
		return -1;

	app.Run();
	app.Shutdown();
	return 0;
}

bool App::Init(HINSTANCE hInstance, int nCmdShow)
{
	if (!window.Create(L"DX12 ImGui App", hInstance, nCmdShow))
		return false;

	if (!renderer.Init(window.GetHWND()))
		return false;

	imgui.Init(window.GetHWND(), renderer);

	auto graph = BuildRenderGraph(renderer, imgui);
	renderer.SetRenderGraph(graph);

	// Center the mouse in the window
	RECT rect;
	GetClientRect(window.GetHWND(), &rect);
	POINT center;
	center.x = (rect.right - rect.left) / 2;
	center.y = (rect.bottom - rect.top) / 2;
	ClientToScreen(window.GetHWND(), &center);
	SetCursorPos(center.x, center.y);

	// Initialize input state
	inputState.lastMouseX = center.x;
	inputState.lastMouseY = center.y;
	inputState.firstMouse = false;

	return true;
}

std::shared_ptr<RenderGraph> App::BuildRenderGraph(Renderer& renderer, ImGuiLayer& imguiLayer) {
	auto graph = std::make_shared<RenderGraph>();

	auto uiPass = std::make_shared<UIRenderPass>(renderer, &imguiLayer);
	auto scenePass = std::make_shared<SceneRenderPass>(&scene, renderer);

	//uiPass->AddDependency(scenePass); // Ensure UI renders after the scene
	// Add other passes and dependencies here as you expand the system

	graph->AddPass(uiPass);
	graph->AddPass(scenePass);
	return graph;
}

void App::Run()
{
	while (!window.ShouldClose()) {
		window.PollEvents();

		UpdateInputState();
		UpdateCameraFromInput();

		renderer.BeginFrame();
		renderer.EndFrame();
	}
}

void App::UpdateInputState() {
    // Keyboard
    for (int i = 0; i < 256; ++i) {
        inputState.keys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }

    // Mouse
    POINT p;
    if (GetCursorPos(&p)) {
        ScreenToClient(window.GetHWND(), &p);
        if (inputState.firstMouse) {
            inputState.lastMouseX = p.x;
            inputState.lastMouseY = p.y;
            inputState.firstMouse = false;
        }
        inputState.mouseDeltaX = p.x - inputState.lastMouseX;
        inputState.mouseDeltaY = p.y - inputState.lastMouseY;
        inputState.lastMouseX = p.x;
        inputState.lastMouseY = p.y;

        // Only recenter if window is focused
        if (GetForegroundWindow() == window.GetHWND()) {
            RECT rect;
            GetClientRect(window.GetHWND(), &rect);
            POINT center;
            center.x = (rect.right - rect.left) / 2;
            center.y = (rect.bottom - rect.top) / 2;
            ClientToScreen(window.GetHWND(), &center);
            SetCursorPos(center.x, center.y);
            inputState.lastMouseX = center.x;
            inputState.lastMouseY = center.y;
        }
    }
}

void App::UpdateCameraFromInput() {
    auto camera = scene.GetActiveCamera();
    if (!camera) return;

    float moveSpeed = 0.1f;
    float rotSpeed = 0.005f;

    if (inputState.keys['W']) camera->MoveForward(moveSpeed);
    if (inputState.keys['S']) camera->MoveForward(-moveSpeed);
    if (inputState.keys['A']) camera->MoveRight(-moveSpeed);
    if (inputState.keys['D']) camera->MoveRight(moveSpeed);

    if (!inputState.firstMouse) {
        camera->Rotate(inputState.mouseDeltaX * rotSpeed, inputState.mouseDeltaY * rotSpeed);
    }
    inputState.mouseDeltaX = 0;
    inputState.mouseDeltaY = 0;
}

void App::Shutdown()
{
	renderer.Shutdown();
	window.Destroy();
}