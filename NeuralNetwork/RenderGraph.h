#include "Renderer.h"
#include "RenderPass.h"
#include <dxgidebug.h>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>

class RenderGraph {
public:
	void AddPass(std::shared_ptr<RenderPass> pass);
	void Execute(Renderer& renderer);
	void Shutdown();

	// Gathers all command lists from each render pass
	std::vector<ID3D12CommandList*> GetAllCommandLists() const {
		std::vector<ID3D12CommandList*> allLists;
		for (const auto& pass : passes) {
			const auto& cmdLists = pass->GetCommandLists();
			allLists.insert(allLists.end(), cmdLists.begin(), cmdLists.end());
		}
		return allLists;
	}
private:
	std::vector<std::shared_ptr<RenderPass>> passes;
};
