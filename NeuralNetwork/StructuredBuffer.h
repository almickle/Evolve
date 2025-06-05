#pragma once
#include <d3d12.h>
#include <string>
#include <vector>
#include "GpuResource.h"

using uint = unsigned int;

template<typename T>
class StructuredBuffer : public GpuResource {
public:
	StructuredBuffer( const std::vector<T>& data, const std::string& debugName = "StructuredBuffer" )
		: GpuResource( D3D12_RESOURCE_STATE_GENERIC_READ ),
		elementSize( sizeof( T ) ),
		elementCount( data.size() )
	{
	}
	~StructuredBuffer()
	{
		data.clear();
	};
public:
	void Update( const void* data, size_t size ) override {};
public:
	uint GetElementSize() const { return elementSize; }
	uint GetElementCount() const { return elementCount; }
	std::vector<T> GetData() { return data; }
private:
	uint elementSize = 0;
	uint elementCount = 0;
	std::vector<T> data;
};