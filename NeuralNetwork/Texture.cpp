#include <d3d12.h>
#include <d3dx12_resource_helpers.h>
#include "GpuResource.h"
#include "Texture.h"

void Texture::Upload( ID3D12GraphicsCommandList* cmdList )
{
	Transition( cmdList, D3D12_RESOURCE_STATE_COPY_DEST );

	UpdateSubresources( cmdList, resource.Get(), uploadResource.Get(), 0, 0, numSubresources, subresourceData.data() );
}

size_t Texture::GetDataSize() const
{
	size_t total = 0;
	for( const auto& sub : subresourceData ) {
		total += sub.SlicePitch;
	}
	return total;
}