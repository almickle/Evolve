#pragma once
#include <d3d12.h>
#include <mutex>
#include <queue>
#include <wrl\client.h>
#include "System.h"
#include "SystemManager.h"
#include "Types.h"

class Renderer;

class DescriptorHeapManager : public System {
public:
	DescriptorHeapManager( SystemManager& systemManager );
	~DescriptorHeapManager() = default;
public:
	void Init( D3D12_DESCRIPTOR_HEAP_TYPE type, uint numDescriptors, bool shaderVisible );
	int Allocate();
	void Free( int index );
public:
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
private:
	Renderer* renderer;
};