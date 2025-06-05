//#include <cassert>
//#include <combaseapi.h>
//#include <cstdint>
//#include <cstring>
//#include <d3d12.h>
//#include <d3dx12_barriers.h>
//#include <d3dx12_core.h>
//#include <dxgiformat.h>
//#include <Windows.h>
//#include "IndexBuffer.h"
//#include "Renderer.h"
//#include "UploadManager.h"
//
//bool IndexBuffer::CreateResource( Renderer& renderer, uint indexCount, DXGI_FORMAT format )
//{
//	this->indexCount = indexCount;
//	this->format = format;
//	uint indexSize = (format == DXGI_FORMAT_R16_uint) ? sizeof( uint16_t ) : sizeof( uint32_t );
//	uint bufferSize = indexCount * indexSize;
//
//	auto device = renderer.GetDevice();
//
//	// 1. Create the default heap resource in COMMON state
//	CD3DX12_HEAP_PROPERTIES heapProps( D3D12_HEAP_TYPE_DEFAULT );
//	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );
//
//	HRESULT hr = device->CreateCommittedResource(
//		&heapProps,
//		D3D12_HEAP_FLAG_NONE,
//		&bufferDesc,
//		D3D12_RESOURCE_STATE_COMMON,
//		nullptr,
//		IID_PPV_ARGS( &resource )
//	);
//	if( FAILED( hr ) ) return false;
//
//	// 2. Create the upload heap
//	CD3DX12_HEAP_PROPERTIES uploadHeapProps( D3D12_HEAP_TYPE_UPLOAD );
//	CD3DX12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );
//
//	hr = device->CreateCommittedResource(
//		&uploadHeapProps,
//		D3D12_HEAP_FLAG_NONE,
//		&uploadBufferDesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS( &uploadHeap )
//	);
//	return SUCCEEDED( hr );
//}
//
//void IndexBuffer::Upload( Renderer& renderer, const void* indexData )
//{
//	uint indexSize = (format == DXGI_FORMAT_R16_uint) ? sizeof( uint16_t ) : sizeof( uint32_t );
//	uint bufferSize = indexCount * indexSize;
//
//	// Copy data to upload heap
//	void* mapped = nullptr;
//	uploadHeap->Map( 0, nullptr, &mapped );
//	memcpy( mapped, indexData, bufferSize );
//	uploadHeap->Unmap( 0, nullptr );
//
//	auto* uploadManager = renderer.GetUploadManager();
//	if( uploadManager ) {
//		uploadManager->Enqueue( {
//			[this, bufferSize]( ID3D12GraphicsCommandList* cmdList ) {
//				CD3DX12_RESOURCE_BARRIER toCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
//					resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST );
//				cmdList->ResourceBarrier( 1, &toCopyDest );
//
//				cmdList->CopyBufferRegion( resource.Get(), 0, uploadHeap.Get(), 0, bufferSize );
//
//				CD3DX12_RESOURCE_BARRIER toFinal = CD3DX12_RESOURCE_BARRIER::Transition(
//					resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER );
//				cmdList->ResourceBarrier( 1, &toFinal );
//
//				ibView.BufferLocation = resource->GetGPUVirtualAddress();
//				ibView.SizeInBytes = bufferSize;
//				ibView.Format = format;
//			},
//			[this]() { ready.store( true, std::memory_order_release ); }
//								} );
//	}
//	else {
//		assert( false && "UploadManager required for async upload" );
//	}
//}