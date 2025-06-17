#include <combaseapi.h>
#include <cstdint>
#include <d3d12.h>
#include <mutex>
#include "DescriptorHeapManager.h"
#include "Renderer.h"
#include "SystemManager.h"
#include "Types.h"

DescriptorHeapManager::DescriptorHeapManager( SystemManager& systemManager )
	: renderer( systemManager.GetRenderer() )
{

}

void DescriptorHeapManager::Init( D3D12_DESCRIPTOR_HEAP_TYPE type, uint numDescriptors, bool shaderVisible, uint reserved )
{
	capacity = numDescriptors;
	reservedCount = reserved;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	renderer->GetDevice()->CreateDescriptorHeap( &desc, IID_PPV_ARGS( &heap ) );
	descriptorSize = renderer->GetDevice()->GetDescriptorHandleIncrementSize( type );

	// Initialize reserved free list
	for( uint i = 0; i < reservedCount; i++ ) {
		reservedFreeList.push( i );
	}
	// Initialize freeList with indices after the reserved region
	for( uint i = reservedCount; i < numDescriptors; i++ ) {
		freeList.push( i );
	}
}

int DescriptorHeapManager::Allocate()
{
	std::lock_guard<std::mutex> lock( allocMutex );
	if( freeList.empty() ) return -1; // Out of descriptors
	int idx = freeList.front();
	freeList.pop();
	return idx;
}

int DescriptorHeapManager::AllocateReserved()
{
	std::lock_guard<std::mutex> lock( allocMutex );
	if( reservedFreeList.empty() ) return -1; // Out of reserved descriptors
	int idx = reservedFreeList.front();
	reservedFreeList.pop();
	return idx;
}

void DescriptorHeapManager::Free( int index )
{
	std::lock_guard<std::mutex> lock( allocMutex );
	freeList.push( index );
}

void DescriptorHeapManager::FreeReserved( int index )
{
	std::lock_guard<std::mutex> lock( allocMutex );
	if( index >= 0 && static_cast<uint>(index) < reservedCount ) {
		reservedFreeList.push( index );
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetCpuHandle( int index ) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (uint64_t)index * descriptorSize;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetGpuHandle( int index ) const
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = heap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += (uint64_t)index * descriptorSize;
	return handle;
}