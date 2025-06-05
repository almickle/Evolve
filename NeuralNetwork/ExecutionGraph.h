#pragma once
#include <d3d12.h>
#include <memory>
#include <vector>
#include "Types.h"

class Renderer;
class GraphPass;

class ExecutionGraph {
public:
	ExecutionGraph() = default;
	~ExecutionGraph() = default;
public:
	virtual void ExecuteSync( Renderer& renderer );
	virtual void ExecuteAsync( Renderer& renderer );
public:
	void AddPass( std::shared_ptr<GraphPass> pass );
	std::vector<ID3D12CommandList*> GetAllCommandLists( uint frameIndex ) const;
	std::vector<std::shared_ptr<GraphPass>> GetPasses() const { return passes; }
public:
	void Shutdown();
protected:
	std::vector<std::shared_ptr<GraphPass>> passes;
};