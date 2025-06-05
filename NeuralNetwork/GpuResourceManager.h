#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>
#include "DataStructures.h"
#include "GpuResource.h"

class Renderer;

using uint = unsigned int;
using byte = uint8_t;
using ResourceID = std::string;

class GpuResourceManager {
public:
	GpuResourceManager( Renderer& renderer );
	~GpuResourceManager();
public:
	bool RegisterResource( std::unique_ptr<GpuResource> resource );
	bool RegisterPerFrameResource( const GpuResource& resource );
	void RemoveResource( const ResourceID& id );
public:
	void CreateVertexBuffer( const std::vector<Vertex>& vertices, const std::string& debugName = "VertexBuffer" );
	void CreateIndexBuffer( const std::vector<uint>& vertices, const std::string& debugName = "IndexBuffer" );
	void CreateConstantBuffer( const std::vector<byte>& data, const std::string& debugName = "ConstantBuffer" );
	//void CreateStructuredBuffer();
	//void CreateTexture2D();
public:
	GpuResource* GetResource( const ResourceID& id );
private:
	Renderer* renderer;
	std::unordered_map<ResourceID, std::unique_ptr<GpuResource>> resourceHeap;
	std::unordered_map<ResourceID, std::vector<std::unique_ptr<GpuResource>>> perFrameResourceHeap;
};