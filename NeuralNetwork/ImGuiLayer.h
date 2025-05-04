#pragma once
#include "Renderer.h"
#include <Windows.h>

class ImGuiLayer {
public:
	void Init(HWND hwnd, Renderer& renderer);
	void BeginFrame();
	void RenderUI();
	void EndFrame(ID3D12GraphicsCommandList* commandList);
	void Shutdown();
};