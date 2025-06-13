#pragma once
#include <d3d12.h>
#include <dxgiformat.h>
#include <string>
#include <vector>
#include "GpuResource.h"
#include "Types.h"

class IndexBuffer : public GpuResource {
public:
	IndexBuffer( const std::vector<uint>& indices, const std::string& name = "IndexBuffer", DXGI_FORMAT format = DXGI_FORMAT_R32_UINT )
		: GpuResource( D3D12_RESOURCE_STATE_INDEX_BUFFER, name ),
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
	const void* GetData() const override { return indices.data(); }
	size_t GetDataSize() const override { return indices.size() * sizeof( uint ); }
public:
	const D3D12_INDEX_BUFFER_VIEW& GetView() const { return ibView; }
	uint GetIndexCount() const { return indexCount; }
	DXGI_FORMAT GetFormat() const { return format; }
	const std::vector<uint>& GetIndices() const { return indices; }
public:
	void SetIndexBufferView( const D3D12_INDEX_BUFFER_VIEW& view ) { ibView = view; }
private:
	D3D12_INDEX_BUFFER_VIEW ibView{};
	uint indexCount = 0;
	DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;
	std::vector<uint> indices;
};