#include <Windows.h>
#include "InputSystem.h"
#include "Window.h"

void InputSystem::UpdateInputState()
{
	// Keyboard
	for( int i = 0; i < 256; ++i ) {
		inputState.keys[i] = (GetAsyncKeyState( i ) & 0x8000) != 0;
	}

	// Mouse
	POINT p;
	HWND hwnd = window->GetHWND();
	if( GetCursorPos( &p ) ) {
		ScreenToClient( hwnd, &p );
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
		if( GetForegroundWindow() == hwnd ) {
			RECT rect;
			GetClientRect( hwnd, &rect );
			POINT center{};
			center.x = (rect.right - rect.left) / 2;
			center.y = (rect.bottom - rect.top) / 2;
			ClientToScreen( hwnd, &center );
			SetCursorPos( center.x, center.y );
			inputState.lastMouseX = center.x;
			inputState.lastMouseY = center.y;
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
	inputState.firstMouse = false;

}

void InputSystem::UpdateCameraFromInput()
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
