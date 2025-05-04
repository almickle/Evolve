#include "Window.h"

bool Window::Create(const wchar_t* title, int width, int height, HINSTANCE hInstance, int nCmdShow)
{
	wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, hInstance, nullptr, nullptr, nullptr, nullptr, title, nullptr };
	::RegisterClassEx(&wc);
	hwnd = ::CreateWindow(wc.lpszClassName, title, WS_OVERLAPPEDWINDOW, 100, 100, width, height, nullptr, nullptr, wc.hInstance, nullptr);

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	::ShowWindow(hwnd, nCmdShow);
	::UpdateWindow(hwnd);

	return hwnd != nullptr;
}

void Window::Destroy()
{
	::DestroyWindow(hwnd);
	hwnd = nullptr;
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

void Window::PollEvents()
{
	MSG msg;
	while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
			quit = true;
	}
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	default:
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}
}