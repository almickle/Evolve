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
private:
	std::vector<std::shared_ptr<RenderPass>> passes;
};
