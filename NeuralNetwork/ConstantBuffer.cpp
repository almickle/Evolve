//#include "ConstantBuffer.h"
//#include "Renderer.h"
//#include <combaseapi.h>
//#include <cstdint>
//#include <cstring>
//#include <d3d12.h>
//#include <d3dx12_core.h>
//#include <Windows.h>
//
//static uint Align256( uint size )
//{
//	return (size + 255) & ~255u;
//}
//
//bool ConstantBuffer::CreateResource( Renderer& renderer, uint size )
//{
//	bufferSize = Align256( size );
//
//	// Create upload heap resource
//	CD3DX12_HEAP_PROPERTIES heapProps( D3D12_HEAP_TYPE_UPLOAD );
//	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );
//
//	auto device = renderer.GetDevice();
//	HRESULT hr = device->CreateCommittedResource(
//		&heapProps,
//		D3D12_HEAP_FLAG_NONE,
//		&desc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS( &resource )
//	);
//	return SUCCEEDED( hr );
//}
//
//void ConstantBuffer::Update( const void* data, uint size )
//{
//	uint alignedSize = Align256( size );
//
//	// Map/Unmap resource directly for updates
//	void* mapped = nullptr;
//	resource->Map( 0, nullptr, &mapped );
//	memcpy( mapped, data, size );
//	// Zero out the padding if any
//	if( alignedSize > size ) {
//		memset( static_cast<uint8_t*>(mapped) + size, 0, alignedSize - size );
//	}
//	resource->Unmap( 0, nullptr );
//}