#pragma once
#include "Scene.h"
#include "System.h"
#include "SystemManager.h"

class Window;

struct InputState {
	bool keys[256] = {};
	int mouseDeltaX = 0;
	int mouseDeltaY = 0;
	int lastMouseX = 0;
	int lastMouseY = 0;
	bool mouseLeftDown = false;
	bool mouseMiddleDown = false;
};

class InputSystem : public System {
public:
	InputSystem( SystemManager& systemManager )
		: window( systemManager.GetWindow() )
	{
	}
public:
	void Update( Scene* scene );
	void CenterMouse();
	void UpdateInputState();
	void UpdateCameraFromInput( Scene* scene );
	const InputState& GetInputState() const { return inputState; }
private:
	InputState inputState;
	Window* window;
};

