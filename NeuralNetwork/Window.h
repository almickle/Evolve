#pragma once
#include <imgui.h>
#include <Windows.h>
#include "System.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

class Window : public System {
public:
	Window() = default;
	~Window()
	{
		::DestroyWindow( hwnd );
		hwnd = nullptr;
		::UnregisterClassW( wc.lpszClassName, wc.hInstance );
	}
public:
	bool Init( const wchar_t* title, HINSTANCE hInstance, int nCmdShow );
	void PollEvents();
	HWND GetHWND() const { return hwnd; }
	bool ShouldClose() const { return quit; }

private:
	WNDCLASSEX wc{};
	HWND hwnd = nullptr;
	bool quit = false;
	static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
};