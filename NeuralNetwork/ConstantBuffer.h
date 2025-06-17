#pragma once
#include <cstring>
#include <d3d12.h>
#include <malloc.h>
#include <memory>
#include <string>
#include "DescriptorHeapManager.h"
#include "GpuResource.h"
#include "Types.h"

class Renderer;

class ConstantBuffer : public GpuResource {
public:
	ConstantBuffer( void* _data, uint _size, const std::string& name = "ConstantBuffer" )
		: GpuResource( D3D12_RESOURCE_STATE_GENERIC_READ, name )
	{
		data = malloc( _size );
		if( data && _data ) {
			memcpy( data, _data, _size );
		}
		size = _size;
	}
	~ConstantBuffer() { free( data ); }
public:
	void Update( const void* data, size_t size ) override;
	void Upload( ID3D12GraphicsCommandList* cmdList ) override;
	std::unique_ptr<GpuResource> Clone( DescriptorHeapManager& srvHeapManager, Renderer& renderer ) const override;
public:
	void* GetData() override { return data; }
private:
	void* data;
};