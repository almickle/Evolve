#include <cassert>
#include <combaseapi.h>
#include <cstring>
#include <d3d12.h>
#include <d3dx12_barriers.h>
#include <d3dx12_core.h>
#include <Windows.h>
#include "GpuResource.h"
#include "GpuResourceManager.h"
#include "Renderer.h"
#include "UploadManager.h"
#include "VertexBuffer.h"

//bool VertexBuffer::CreateResource( Renderer& renderer, uint vertexCount, uint vertexStride )
//{
//	this->vertexCount = vertexCount;
//	this->vertexStride = vertexStride;
//	uint bufferSize = vertexCount * vertexStride;
//
//	// Get the resource manager
//	auto resourceManager = renderer.GetGpuResourceManager();
//	assert( resourceManager );
//
//	// Generate unique IDs for this buffer
//	resourceId = "VertexBuffer_" + std::to_string( reinterpret_cast<uintptr_t>(this) );
//	uploadHeapId = "VertexBufferUpload_" + std::to_string( reinterpret_cast<uintptr_t>(this) );
//
//	// Create the default heap resource (per-frame)
//	{
//		GpuResource vbTemplate( D3D12_RESOURCE_STATE_COMMON );
//		// You may want to add a constructor or method to GpuResource to set up the resource description
//		// For now, assume GpuResourceManager will handle the actual D3D12 resource creation
//		resourceManager->CreatePerFrameResource( vbTemplate );
//	}
//
//	// Create the upload heap (per-frame)
//	{
//		GpuResource uploadTemplate( uploadHeapId, D3D12_RESOURCE_STATE_GENERIC_READ, uploadHeapId );
//		resourceManager->CreatePerFrameResource( uploadHeapId, uploadTemplate );
//	}
//
//	return true;
//}

//void VertexBuffer::Upload( Renderer& renderer, const void* vertexData )
//{
//	uint bufferSize = vertexCount * vertexStride;
//
//	// Copy data to upload heap
//	void* mapped = nullptr;
//	auto uploadHeapResource = resourceManager->GetResource( uploadHeapId );
//	assert( uploadHeapResource );
//	uploadHeapResource->Map( 0, nullptr, &mapped );
//	memcpy( mapped, vertexData, bufferSize );
//	uploadHeapResource->Unmap( 0, nullptr );
//
//	auto* uploadManager = renderer.GetUploadManager();
//	if( uploadManager ) {
//		uploadManager->Enqueue( {
//			[this, bufferSize]( ID3D12GraphicsCommandList* cmdList ) {
//				auto resource = resourceManager->GetResource( resourceId );
//				assert( resource );
//
//				CD3DX12_RESOURCE_BARRIER toCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
//					resource->Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST );
//				cmdList->ResourceBarrier( 1, &toCopyDest );
//
//				cmdList->CopyBufferRegion( resource->Get(), 0, resourceManager->GetResource( uploadHeapId )->Get(), 0, bufferSize );
//
//				CD3DX12_RESOURCE_BARRIER toFinal = CD3DX12_RESOURCE_BARRIER::Transition(
//					resource->Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER );
//				cmdList->ResourceBarrier( 1, &toFinal );
//
//				vbView.BufferLocation = resource->GetGPUVirtualAddress();
//				vbView.SizeInBytes = bufferSize;
//				vbView.StrideInBytes = vertexStride;
//			},
//			[this]() { state.ready.store( true, std::memory_order_release ); }
//								} );
//	}
//	else {
//		assert( false && "UploadManager required for async upload" );
//	}
//}