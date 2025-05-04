#pragma once
#include <memory>
#include <string>
#include <vector>

class Renderer;

class RenderPass {
public:
	RenderPass(const std::string& name) : name(name) {}
	virtual ~RenderPass() = default;
public:
	virtual void Execute(Renderer& renderer) = 0;
	void AddDependency(std::shared_ptr<RenderPass> dependency);
	const std::vector<RenderPass*>& GetDependencies() const;
	const std::string& GetName() const;
protected:
	std::string name;
	std::vector<RenderPass*> dependencies;
};
