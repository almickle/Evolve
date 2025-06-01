#pragma once
#include "Renderer.h"
#include <memory>
#include <string>
#include <vector>
#include <wrl/client.h>

class RenderPass {
public:
	RenderPass(const std::string& name) : name(name) {}
	virtual ~RenderPass() = default;
public:
	virtual void Execute(Renderer& renderer) = 0;
	virtual void Shutdown() = 0;
	void AddDependency(std::shared_ptr<RenderPass> dependency);
	const std::vector<RenderPass*>& GetDependencies() const;
	const std::string& GetName() const;
	void AddCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& allocator);
	std::vector<ID3D12CommandList*> GetCommandLists() const {
		std::vector<ID3D12CommandList*> lists;
		for (const auto& cmd : commandLists) {
			lists.push_back(cmd.Get());
		}
		return lists;
	}

protected:
	std::string name;
	std::vector<RenderPass*> dependencies;
	std::vector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> commandLists;
	std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> commandAllocators;
};
