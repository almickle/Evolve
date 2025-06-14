#pragma once
#include <d3d12.h>
#include <string>
#include <vector>
#include <wrl/client.h>
#include "Renderer.h"
#include "Types.h"

class SystemManager;

class GraphPass {
	struct PassState {
		bool notStarted;
		bool running;
		bool finished;
	};
public:
	GraphPass( const std::string& name = "GraphPass" )
		: name( name )
	{
	}
	GraphPass() = default;
	virtual ~GraphPass() = default;
public:
	virtual void Init( SystemManager& systemManager ) = 0;
	virtual void Execute( SystemManager& systemManager ) = 0;
public:
	bool IsReady() const;
	bool IsFinished() const { return state.finished; }
	bool IsRunning() const { return state.running; }
	bool IsNotStarted() const { return state.notStarted; }
	const std::string& GetName() const { return name; };
	const std::vector<GraphPass*>& GetDependencies() const { return dependencies; }
	ID3D12CommandList* GetCurrentCommandList( uint& frameIndex ) const;
public:
	GraphPass* AddDependency( GraphPass* dependency );
	void RemoveDependency( GraphPass* dependency );
protected:
	PassState state = { true, false, false };
	std::string name;
	std::vector<GraphPass*> dependencies;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandLists[Renderer::BackBufferCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[Renderer::BackBufferCount];
};
