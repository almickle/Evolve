//#include "StructuredBuffer.h"
//#include "Renderer.h"
//#include "UploadManager.h"
//#include "d3dx12.h"
//
//bool StructuredBuffer::CreateResource(Renderer& renderer, uint elementSize, uint elementCount) {
//    this->elementSize = elementSize;
//    this->elementCount = elementCount;
//    uint64 bufferSize = static_cast<uint64>(elementSize) * elementCount;
//
//    // Create default heap resource
//    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
//    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);
//
//    auto device = renderer.GetDevice();
//    HRESULT hr = device->CreateCommittedResource(
//        &heapProps,
//        D3D12_HEAP_FLAG_NONE,
//        &desc,
//        D3D12_RESOURCE_STATE_COMMON,
//        nullptr,
//        IID_PPV_ARGS(&resource)
//    );
//    if (FAILED(hr)) return false;
//
//    // Create upload heap
//    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
//    CD3DX12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
//    hr = device->CreateCommittedResource(
//        &uploadHeapProps,
//        D3D12_HEAP_FLAG_NONE,
//        &uploadDesc,
//        D3D12_RESOURCE_STATE_GENERIC_READ,
//        nullptr,
//        IID_PPV_ARGS(&uploadHeap)
//    );
//
//    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
//    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
//    srvDesc.Buffer.FirstElement = 0;
//    srvDesc.Buffer.NumElements = elementCount;
//    srvDesc.Buffer.StructureByteStride = elementSize;
//    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//
//    CreateSRV(renderer, srvDesc);
//
//    return SUCCEEDED(hr);
//}
//
//void StructuredBuffer::Upload(Renderer& renderer, const void* data, uint elementCount) {
//    uint64 bufferSize = static_cast<uint64>(elementSize) * elementCount;
//
//    // Copy data to upload heap
//    void* mapped = nullptr;
//    uploadHeap->Map(0, nullptr, &mapped);
//    memcpy(mapped, data, bufferSize);
//    uploadHeap->Unmap(0, nullptr);
//
//    auto* uploadManager = renderer.GetUploadManager();
//    if (uploadManager) {
//        uploadManager->Enqueue({
//            [this, bufferSize](ID3D12GraphicsCommandList* cmdList) {
//                // Transition to COPY_DEST
//                CD3DX12_RESOURCE_BARRIER toCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
//                    resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
//                cmdList->ResourceBarrier(1, &toCopyDest);
//
//                // Copy from upload heap to default heap
//                cmdList->CopyBufferRegion(resource.Get(), 0, uploadHeap.Get(), 0, bufferSize);
//
//                // Transition to GENERIC_READ
//                CD3DX12_RESOURCE_BARRIER toRead = CD3DX12_RESOURCE_BARRIER::Transition(
//                    resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
//                cmdList->ResourceBarrier(1, &toRead);
//            },
//            [this]() { ready.store(true, std::memory_order_release); }
//        });
//    }
//}