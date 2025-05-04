#pragma once
#include "ImGuiLayer.h"
#include "RenderPass.h"

class ImGuiRenderPass : public RenderPass {
public:
	ImGuiRenderPass(ImGuiLayer* layer) : RenderPass("ImGuiPass"), imguiLayer(layer) {}
	void Execute(Renderer& renderer) override {
		imguiLayer->BeginFrame();
		imguiLayer->RenderUI();
		imguiLayer->EndFrame(renderer.GetCommandList());
	}
private:
	ImGuiLayer* imguiLayer;
};