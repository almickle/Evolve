//#include "Landscape.h"
//#include <DirectXTex.h>
//#include <cassert>
//#include <algorithm>
//#include <filesystem>
//
//void SmoothHeightmap(std::vector<std::vector<float>>& heightmap, int iterations = 1) {
//    size_t h = heightmap.size();
//    size_t w = heightmap[0].size();
//    std::vector<std::vector<float>> temp = heightmap;
//    for (int it = 0; it < iterations; ++it) {
//        for (size_t y = 1; y < h - 1; ++y) {
//            for (size_t x = 1; x < w - 1; ++x) {
//                temp[y][x] = (
//                    heightmap[y][x] +
//                    heightmap[y - 1][x] + heightmap[y + 1][x] +
//                    heightmap[y][x - 1] + heightmap[y][x + 1]
//                    ) / 5.0f;
//            }
//        }
//        heightmap.swap(temp);
//    }
//}
//
//// Helper function to generate mesh data from a DDS image
//static void GenerateLandscapeMesh(
//    const std::string& imagePath,
//    float scale,
//    float heightScale,
//    std::vector<Vertex>& outVertices,
//    std::vector<uint32_t>& outIndices)
//{
//    DirectX::ScratchImage image;
//    std::filesystem::path path(imagePath);
//    HRESULT hr = DirectX::LoadFromDDSFile(
//        path.wstring().c_str(),
//        DirectX::DDS_FLAGS_NONE,
//        nullptr,
//        image
//    );
//    assert(SUCCEEDED(hr) && "Failed to load heightmap image");
//
//    const DirectX::Image* img = image.GetImage(0, 0, 0);
//    assert(img && "No image data");
//
//    auto format = img->format;
//
//    size_t width = img->width;
//    size_t height = img->height;
//    assert(width > 1 && height > 1);
//
//    // Convert image to height values (normalized 0..1)
//    std::vector<std::vector<float>> heightmap(height, std::vector<float>(width));
//    for (size_t y = 0; y < height; ++y) {
//        for (size_t x = 0; x < width; ++x) {
//            float gray = 0.0f;
//            switch (img->format) {
//            case DXGI_FORMAT_R8_UNORM: {
//                // 8-bit single channel
//                const uint8_t* row = img->pixels + y * img->rowPitch;
//                gray = row[x] / 255.0f;
//                break;
//            }
//            case DXGI_FORMAT_R8G8B8A8_UNORM: {
//                // 8-bit RGBA
//                const uint8_t* row = img->pixels + y * img->rowPitch;
//                gray = row[x * 4] / 255.0f; // Use red channel
//                break;
//            }
//            case DXGI_FORMAT_R32_FLOAT: {
//                // 32-bit float single channel
//                const float* row = reinterpret_cast<const float*>(img->pixels + y * img->rowPitch);
//                gray = row[x];
//                break;
//            }
//            default:
//                assert(false && "Unsupported DDS format for heightmap");
//                break;
//            }
//            heightmap[y][x] = gray;
//        }
//    }
//
//    // Smooth the heightmap
//    SmoothHeightmap(heightmap, 1);
//
//    // Generate vertices
//    outVertices.clear();
//    for (size_t z = 0; z < height; ++z) {
//        for (size_t x = 0; x < width; ++x) {
//            float fx = static_cast<float>(x) * scale;
//            float fy = static_cast<float>(z) * scale;
//            float fz = heightmap[z][x] * heightScale;
//
//            Vertex v;
//            v.position = { fx, fy, fz };
//            v.texcoord = { x / float(width - 1), z / float(height - 1) };
//            v.normal = { 0, 1, 0 }; // Temporary, will compute below
//            outVertices.push_back(v);
//        }
//    }
//
//    // Generate indices (two triangles per quad) - CCW for Z-up
//    outIndices.clear();
//    for (size_t z = 0; z < height - 1; ++z) {
//        for (size_t x = 0; x < width - 1; ++x) {
//            uint32_t i0 = uint32_t(z * width + x);
//            uint32_t i1 = uint32_t(z * width + x + 1);
//            uint32_t i2 = uint32_t((z + 1) * width + x);
//            uint32_t i3 = uint32_t((z + 1) * width + x + 1);
//
//            // Triangle 1 (i0, i1, i2)
//            outIndices.push_back(i0);
//            outIndices.push_back(i1);
//            outIndices.push_back(i2);
//
//            // Triangle 2 (i1, i3, i2)
//            outIndices.push_back(i1);
//            outIndices.push_back(i3);
//            outIndices.push_back(i2);
//        }
//    }
//
//    // Compute normals
//    for (size_t z = 0; z < height - 1; ++z) {
//        for (size_t x = 0; x < width - 1; ++x) {
//            uint32_t i0 = uint32_t(z * width + x);
//            uint32_t i1 = uint32_t(z * width + x + 1);
//            uint32_t i2 = uint32_t((z + 1) * width + x);
//
//            auto& v0 = outVertices[i0];
//            auto& v1 = outVertices[i1];
//            auto& v2 = outVertices[i2];
//
//            DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&v0.position);
//            DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&v1.position);
//            DirectX::XMVECTOR p2 = DirectX::XMLoadFloat3(&v2.position);
//
//            DirectX::XMVECTOR e1 = DirectX::XMVectorSubtract(p1, p0);
//            DirectX::XMVECTOR e2 = DirectX::XMVectorSubtract(p2, p0);
//            DirectX::XMVECTOR n = DirectX::XMVector3Cross(e1, e2);
//
//            DirectX::XMFLOAT3 normal;
//            DirectX::XMStoreFloat3(&normal, n);
//
//            v0.normal.x += normal.x;
//            v0.normal.y += normal.y;
//            v0.normal.z += normal.z;
//            v1.normal.x += normal.x;
//            v1.normal.y += normal.y;
//            v1.normal.z += normal.z;
//            v2.normal.x += normal.x;
//            v2.normal.y += normal.y;
//            v2.normal.z += normal.z;
//        }
//    }
//    // Normalize normals
//    for (auto& v : outVertices) {
//        DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&v.normal);
//        n = DirectX::XMVector3Normalize(n);
//        DirectX::XMStoreFloat3(&v.normal, n);
//    }
//}
//
//Landscape::Landscape(const std::string& imagePath, float scale, float heightScale)
//    : Mesh([&] {
//        std::vector<Vertex> vertices;
//        std::vector<uint32_t> indices;
//        GenerateLandscapeMesh(imagePath, scale, heightScale, vertices, indices);
//        return vertices;
//    }(), [&] {
//        std::vector<Vertex> vertices;
//        std::vector<uint32_t> indices;
//        GenerateLandscapeMesh(imagePath, scale, heightScale, vertices, indices);
//        return indices;
//    }(), "Landscape")
//{}