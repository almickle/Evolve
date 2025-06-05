#pragma once
#include <d3d12.h>
#include <mutex>
#include <queue>
#include <wrl\client.h>

using uint = unsigned int;

class DescriptorHeapManager {
public:
	DescriptorHeapManager( ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint numDescriptors, bool shaderVisible );

	int Allocate();
	void Free( int index );

	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle( int index ) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle( int index ) const;

	ID3D12DescriptorHeap* GetHeap() const { return heap.Get(); }
	uint GetDescriptorSize() const { return descriptorSize; }

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
	uint descriptorSize = 0;
	uint capacity = 0;
	std::queue<int> freeList;
	std::mutex allocMutex;
};