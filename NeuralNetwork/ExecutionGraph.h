#pragma once
#include <d3d12.h>
#include <memory>
#include <vector>
#include "GraphPass.h"
#include "Types.h"

class SystemManager;

class ExecutionGraph {
public:
	ExecutionGraph() {};
	~ExecutionGraph() = default;
public:
	virtual void ExecuteSync( SystemManager& systemManager );
	virtual void ExecuteAsync( SystemManager& systemManager );
public:
	ExecutionGraph* AddPass( std::unique_ptr<GraphPass> pass );
	std::vector<ID3D12CommandList*> GetAllCommandLists( uint frameIndex ) const;
	const std::vector<std::unique_ptr<GraphPass>>& GetPasses() const { return passes; }
protected:
	std::vector<std::unique_ptr<GraphPass>> passes;
};