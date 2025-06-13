#pragma once
#include <d3d12.h>
#include <memory>
#include <vector>
#include "Types.h"

class SystemManager;
class GraphPass;

class ExecutionGraph {
public:
	ExecutionGraph() = default;
	~ExecutionGraph() = default;
public:
	virtual void ExecuteSync( SystemManager& systemManager );
	virtual void ExecuteAsync( SystemManager& systemManager );
public:
	void AddPass( std::shared_ptr<GraphPass> pass );
	std::vector<ID3D12CommandList*> GetAllCommandLists( uint frameIndex ) const;
	std::vector<std::shared_ptr<GraphPass>> GetPasses() const { return passes; }
protected:
	std::vector<std::shared_ptr<GraphPass>> passes;
};