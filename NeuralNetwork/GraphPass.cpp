#include <d3d12.h>
#include <memory>
#include <stdexcept>
#include <vector>
#include <Windows.h>
#include <wrl\client.h>
#include "GraphPass.h"
#include "Renderer.h"

void GraphPass::AddDependency( std::shared_ptr<GraphPass> dependency )
{
	dependencies.push_back( dependency.get() );
}

void GraphPass::RemoveDependency( GraphPass* dependency )
{
	dependencies.erase(
		std::remove( dependencies.begin(), dependencies.end(), dependency ),
		dependencies.end()
	);
}

ID3D12CommandList* GraphPass::GetCurrentCommandList( uint& frameIndex ) const
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