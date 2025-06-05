#pragma once
#include <d3d12.h>
#include <Windows.h>

class Renderer;

class ImGuiLayer {
public:
	void Init( HWND hwnd, Renderer& renderer );
	void BeginFrame();
	void RenderUI();
	void EndFrame( ID3D12GraphicsCommandList* commandList );
	void Shutdown();

private:
	int imguiFontSrvIndex = -1;
};