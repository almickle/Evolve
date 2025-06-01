#include "RenderPass.h"

void RenderPass::AddDependency(std::shared_ptr<RenderPass> dependency) {
	dependencies.push_back(dependency.get());
}

void RenderPass::AddCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& allocator) {
	commandLists.push_back(commandList);
	commandAllocators.push_back(allocator);
}

const std::vector<RenderPass*>& RenderPass::GetDependencies() const {
	return dependencies;
}

const std::string& RenderPass::GetName() const { return name; }