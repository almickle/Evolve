//#pragma once
//#include <wrl/client.h>
//#include <d3d12.h>
//#include <cstdint>
//#include "GpuResource.h"
//#include "Renderer.h"
//
//class StructuredBuffer : public GpuResource {
//public:
//    StructuredBuffer()
//        : GpuResource(D3D12_RESOURCE_STATE_GENERIC_READ) // Default state for upload
//    {}
//    ~StructuredBuffer() = default;
//
//    // Create the buffer with a given element size and count
//    bool CreateResource(Renderer& renderer, uint elementSize, uint elementCount);
//
//    // Upload data to the buffer using the UploadManager
//    void Upload(Renderer& renderer, const void* data, uint elementCount);
//
//    uint GetElementSize() const { return elementSize; }
//    uint GetElementCount() const { return elementCount; }
//
//private:
//    uint elementSize = 0;
//    uint elementCount = 0;
//};