#pragma once
#include <d3d12.h>
#include <memory>
#include <string>
#include <vector>
#include <Windows.h>
#include <wrl/client.h>
#include "Renderer.h"

class GraphPass {
	struct PassState {
		bool notStarted;
		bool running;
		bool finished;
	};
public:
	GraphPass( const std::string& name )
		: name( name )
	{
	}
	virtual ~GraphPass() = default;
public:
	virtual void Init( Renderer& renderer ) = 0;
	virtual void Execute( Renderer& renderer ) = 0;
	virtual void Shutdown() = 0;
public:
	bool IsReady() const;
	bool IsFinished() const { return state.finished; }
	bool IsRunning() const { return state.running; }
	bool IsNotStarted() const { return state.notStarted; }
	const std::string& GetName() const { return name; };
	const std::vector<GraphPass*>& GetDependencies() const { return dependencies; }
	ID3D12CommandList* GetCurrentCommandList( uint& frameIndex ) const;
public:
	void AddDependency( std::shared_ptr<GraphPass> dependency );
	void RemoveDependency( GraphPass* dependency );
protected:
	PassState state = { true, false, false };
	std::string name;
	std::vector<GraphPass*> dependencies;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandLists[Renderer::BackBufferCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[Renderer::BackBufferCount];
};
