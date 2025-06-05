#pragma once
#include <cstdint>
#include <d3d12.h>
#include <dxgiformat.h>
#include <string>
#include <vector>
#include "GpuResource.h"
#include "Types.h"

class Renderer;

class IndexBuffer : public GpuResource {
public:
	IndexBuffer( const std::vector<uint32_t>& indices, const std::string& debugName = "IndexBuffer", DXGI_FORMAT format = DXGI_FORMAT_R32_UINT )
		: GpuResource( D3D12_RESOURCE_STATE_INDEX_BUFFER, debugName ),
		indexCount( static_cast<uint>(indices.size()) ),
		indices( indices ),
		format( format )
	{
	}
	~IndexBuffer()
	{
		indices.clear();
	}
public:
	const D3D12_INDEX_BUFFER_VIEW& GetView() const { return ibView; }
	uint GetIndexCount() const { return indexCount; }
	DXGI_FORMAT GetFormat() const { return format; }
	const std::vector<uint32_t>& GetIndices() const { return indices; }
private:
	D3D12_INDEX_BUFFER_VIEW ibView{};
	uint indexCount = 0;
	DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;
	std::vector<uint32_t> indices;
};