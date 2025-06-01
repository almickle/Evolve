#pragma once
#include "ImGuiLayer.h"
#include "RenderPass.h"

class UIRenderPass : public RenderPass {
public:
	UIRenderPass(Renderer& renderer, ImGuiLayer* layer);
	void Execute(Renderer& renderer) override;
	void Shutdown() override;
private:
	ImGuiLayer* imguiLayer;
};