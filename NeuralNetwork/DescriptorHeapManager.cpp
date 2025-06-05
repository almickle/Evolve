#include "DescriptorHeapManager.h"

DescriptorHeapManager::DescriptorHeapManager(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint numDescriptors, bool shaderVisible)
    : capacity(numDescriptors)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = numDescriptors;
    desc.Type = type;
    desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
    descriptorSize = device->GetDescriptorHandleIncrementSize(type);

    // Fill free list
    for (int i = 0; i < (int)numDescriptors; ++i)
        freeList.push(i);
}

int DescriptorHeapManager::Allocate() {
    std::lock_guard<std::mutex> lock(allocMutex);
    if (freeList.empty()) return -1; // Out of descriptors
    int idx = freeList.front();
    freeList.pop();
    return idx;
}

void DescriptorHeapManager::Free(int index) {
    std::lock_guard<std::mutex> lock(allocMutex);
    freeList.push(index);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetCpuHandle(int index) const {
    D3D12_CPU_DESCRIPTOR_HANDLE handle = heap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSize;
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetGpuHandle(int index) const {
    D3D12_GPU_DESCRIPTOR_HANDLE handle = heap->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSize;
    return handle;
}