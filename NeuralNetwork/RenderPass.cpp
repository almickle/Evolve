#include "RenderPass.h"

void RenderPass::AddDependency(std::shared_ptr<RenderPass> dependency) {
	dependencies.push_back(dependency.get());
}

const std::vector<RenderPass*>& RenderPass::GetDependencies() const {
	return dependencies;
}

const std::string& RenderPass::GetName() const { return name; }