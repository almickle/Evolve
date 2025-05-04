#pragma once
#include "imgui_impl_win32.h"
#include <Windows.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Window {
public:
	bool Create(const wchar_t* title, int width, int height, HINSTANCE hInstance, int nCmdShow);
	void Destroy();
	void PollEvents();
	HWND GetHWND() const { return hwnd; }
	bool ShouldClose() const { return quit; }

private:
	WNDCLASSEX wc;
	HWND hwnd = nullptr;
	bool quit = false;
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
};