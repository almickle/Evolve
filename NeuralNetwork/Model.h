#pragma once
#include <array>
#include <d3d12.h>
#include <string>
#include <vector>
#include "Asset.h"
#include "AssetManager.h"
#include "ConstantBuffer.h"
#include "GpuResourceManager.h"
#include "JsonSerializer.h"
#include "Material.h"
#include "Mesh.h"
#include "Renderer.h"
#include "SystemManager.h"
#include "Types.h"

class Model : public Asset {
public:
	Model( const std::string& name = "Model" )
		: Asset( AssetType::Model, name )
	{
	}
	~Model() = default;
public:
	void Load( SystemManager* systemManager, JsonSerializer& serializer ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
	void AddPsoKey( const PipelineStateKey& psoKey ) { psoKeys.push_back( psoKey ); };
public:
	std::vector<AssetID>& GetMaterialSlots() { return materialSlots; }
	std::vector<AssetID>& GetModifierSlots() { return modifierSlots; }
	AssetID& GetMeshID() { return meshId; }
public:
	std::vector<PipelineStateKey>& GetPsoKeys() { return psoKeys; }
	std::vector<D3D12_VERTEX_BUFFER_VIEW*> GetVertexBufferViews( AssetManager& assetManager, GpuResourceManager& resourceManager ) const
	{
		return static_cast<Mesh*>(assetManager.GetAsset( meshId ))->GetVertexBufferViews( resourceManager );
	}
	std::vector<D3D12_INDEX_BUFFER_VIEW*> GetIndexBufferViews( AssetManager& assetManager, GpuResourceManager& resourceManager ) const
	{
		return static_cast<Mesh*>(assetManager.GetAsset( meshId ))->GetIndexBufferViews( resourceManager );
	}
	std::vector<std::array<ConstantBuffer*, 3>> GetMaterialConstantBuffers( AssetManager& assetManager, GpuResourceManager& resourceManager ) const
	{
		std::vector<std::array<ConstantBuffer*, 3>> materialConstantBuffers;
		materialConstantBuffers.reserve( materialSlots.size() );
		for( const auto& materialId : materialSlots )
		{
			auto material = static_cast<Material*>(assetManager.GetAsset( materialId ));
			if( material != nullptr )
			{
				materialConstantBuffers.push_back( material->GetConstantBuffers( resourceManager ) );
			}
		}
		return materialConstantBuffers;
	}
	std::vector<std::array<D3D12_GPU_VIRTUAL_ADDRESS, 3>> GetMaterialConstantBufferAddresses( AssetManager& assetManager, GpuResourceManager& resourceManager ) const
	{
		std::vector<std::array<D3D12_GPU_VIRTUAL_ADDRESS, 3>> materialConstantBufferAddresses;
		materialConstantBufferAddresses.reserve( materialSlots.size() );
		for( const auto& materialId : materialSlots )
		{
			auto material = static_cast<Material*>(assetManager.GetAsset( materialId ));
			if( material != nullptr )
			{
				auto cbs = material->GetConstantBuffers( resourceManager );
				materialConstantBufferAddresses.push_back( { cbs[0]->GetGpuAddress(), cbs[1]->GetGpuAddress(), cbs[2]->GetGpuAddress() } );
			}
		}
		return materialConstantBufferAddresses;
	}
private:
	AssetID meshId;
	std::vector<AssetID> materialSlots;
	std::vector<AssetID> modifierSlots;
	std::vector<PipelineStateKey> psoKeys;
};

