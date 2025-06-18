#include <combaseapi.h>
#include <d3d12.h>
#include <memory>
#include <vector>
#include <wrl\client.h>
#include "GraphPass.h"
#include "Renderer.h"
#include "SystemManager.h"
#include "Types.h"

GraphPass* GraphPass::AddDependency( GraphPass* dependency )
{
	dependencies.push_back( dependency );

	return this;
}

void GraphPass::RemoveDependency( GraphPass* dependency )
{
	dependencies.erase(
		std::remove( dependencies.begin(), dependencies.end(), dependency ),
		dependencies.end()
	);
}

ID3D12CommandList* GraphPass::GetCurrentCommandList( const uint& frameIndex ) const
{
	return commandLists[frameIndex].Get();
}

bool GraphPass::IsReady() const
{
	for( const auto* dep : dependencies ) {
		if( !dep->IsFinished() ) {
			return false;
		}
	}
	return true;
}

void GraphPass::Init( SystemManager& systemManager )
{
	uint frames = Renderer::BackBufferCount;

	for( uint i = 0; i < frames; ++i ) {
		systemManager.GetRenderer()->GetDevice()->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS( &commandAllocators[i] )
		);
		systemManager.GetRenderer()->GetDevice()->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocators[i].Get(),
			nullptr,
			IID_PPV_ARGS( &commandLists[i] )
		);
		commandLists[i]->Close();
	}
}