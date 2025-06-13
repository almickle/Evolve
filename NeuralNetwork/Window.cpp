#include <combaseapi.h>
#include <objbase.h>
#include <Windows.h>
#include "Window.h"

bool Window::Init( const wchar_t* title, HINSTANCE hInstance, int nCmdShow )
{
	wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, WndProc, 0L, 0L, hInstance, nullptr, nullptr, nullptr, nullptr, title, nullptr };
	::RegisterClassEx( &wc );

	// Get primary monitor resolution
	MONITORINFO mi = { sizeof( mi ) };
	GetMonitorInfo( MonitorFromPoint( { 0, 0 }, MONITOR_DEFAULTTOPRIMARY ), &mi );
	int monitorWidth = mi.rcMonitor.right - mi.rcMonitor.left;
	int monitorHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;

	// Create window using full monitor dimensions
	hwnd = ::CreateWindow( wc.lpszClassName, title, WS_POPUP,
						   mi.rcMonitor.left, mi.rcMonitor.top,
						   monitorWidth, monitorHeight,
						   nullptr, nullptr, wc.hInstance, nullptr );

	if( !hwnd )
		return false;

	// Show the window
	::ShowWindow( hwnd, SW_SHOW );
	::UpdateWindow( hwnd );

	// COM init
	HRESULT hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );

	return hwnd != nullptr;
}

void Window::PollEvents()
{
	MSG msg;
	while( ::PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) ) {
		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
		if( msg.message == WM_QUIT )
			quit = true;
	}
}

LRESULT CALLBACK Window::WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) )
		return true;

	switch( msg ) {
		case WM_DESTROY:
			::PostQuitMessage( 0 );
			return 0;
		default:
			return ::DefWindowProc( hWnd, msg, wParam, lParam );
	}
}