#pragma once
#include <cstdint>
#include <d3d12.h>
#include <string>
#include <vector>
#include "GpuResource.h"

class Renderer;

using uint = unsigned int;
using byte = uint8_t;

class ConstantBuffer : public GpuResource {
public:
	ConstantBuffer( const std::vector<byte>& data, const std::string& debugName = "ConstantBuffer" )
		: GpuResource( D3D12_RESOURCE_STATE_GENERIC_READ, debugName ),
		bufferData( data ),
		bufferSize( static_cast<uint>(data.size()) )
	{
		bufferData.clear();
	}
	~ConstantBuffer() = default;
public:
	void Update( const void* data, size_t size ) override {};
public:
	const std::vector<byte>& GetData() const { return bufferData; }
	uint GetBufferSize() const { return bufferSize; }
private:
	std::vector<byte> bufferData;
	uint bufferSize = 0;
};