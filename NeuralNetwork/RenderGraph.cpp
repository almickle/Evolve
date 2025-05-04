#include "RenderGraph.h"

void RenderGraph::AddPass(std::shared_ptr<RenderPass> pass) {
	passes.push_back(pass);
}

void RenderGraph::Execute(Renderer& renderer) {
	std::unordered_map<RenderPass*, int> indegree;
	std::unordered_map<RenderPass*, std::vector<RenderPass*>> adj;
	std::queue<RenderPass*> ready;

	for (auto& pass : passes) {
		indegree[pass.get()] = 0;
	}

	for (auto& pass : passes) {
		for (auto* dep : pass->GetDependencies()) {
			adj[dep].push_back(pass.get());
			indegree[pass.get()]++;
		}
	}

	for (auto& [node, deg] : indegree) {
		if (deg == 0) ready.push(node);
	}

	while (!ready.empty()) {
		RenderPass* pass = ready.front();
		ready.pop();

		pass->Execute(renderer);

		for (RenderPass* neighbor : adj[pass]) {
			if (--indegree[neighbor] == 0) {
				ready.push(neighbor);
			}
		}
	}
}