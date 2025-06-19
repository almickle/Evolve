#include <Windows.h>
#include "InputSystem.h"
#include "Scene.h"
#include "Window.h"

void InputSystem::Update( Scene* scene )
{
	UpdateInputState();
	UpdateCameraFromInput( scene );
}

void InputSystem::UpdateInputState()
{
	HWND hwnd = window->GetHWND();
	if( GetForegroundWindow() == hwnd ) {

		// Keyboard
		for( int i = 0; i < 256; ++i ) {
			inputState.keys[i] = (GetAsyncKeyState( i ) & 0x8000) != 0;
		}

		// Mouse
		POINT p;
		if( GetCursorPos( &p ) ) {
			inputState.mouseDeltaX = p.x - inputState.lastMouseX;
			inputState.mouseDeltaY = p.y - inputState.lastMouseY;
			inputState.lastMouseX = p.x;
			inputState.lastMouseY = p.y;
			inputState.mouseLeftDown = (GetAsyncKeyState( VK_LBUTTON ) & 0x8000) != 0;
			inputState.mouseMiddleDown = (GetAsyncKeyState( VK_MBUTTON ) & 0x8000) != 0;
		}
	}
}

void InputSystem::CenterMouse()
{
	// Center the mouse in the window
	RECT rect;
	GetClientRect( window->GetHWND(), &rect );
	POINT center{};
	center.x = (rect.right - rect.left) / 2;
	center.y = (rect.bottom - rect.top) / 2;
	ClientToScreen( window->GetHWND(), &center );
	//SetCursorPos( center.x, center.y );

	// Initialize input state
	inputState.lastMouseX = center.x;
	inputState.lastMouseY = center.y;
}

void InputSystem::UpdateCameraFromInput( Scene* scene )
{
	auto* camera = scene->GetActiveCamera();
	if( !camera ) return;

	if( inputState.keys['W'] ) camera->MoveForward();
	if( inputState.keys['S'] ) camera->MoveBack();
	if( inputState.keys['A'] ) camera->MoveLeft();
	if( inputState.keys['D'] ) camera->MoveRight();

	if( inputState.mouseLeftDown ) {
		// Rotate camera
		camera->Rotate( inputState.mouseDeltaX, inputState.mouseDeltaY );
	}
	if( inputState.mouseMiddleDown ) {
		// Pan camera: move right and up in camera space
		camera->MoveRight( inputState.mouseDeltaX );
		camera->MoveUp( -inputState.mouseDeltaY );
	}
	inputState.mouseDeltaX = 0;
	inputState.mouseDeltaY = 0;
}
