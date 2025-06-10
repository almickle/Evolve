//#include <cassert>
//#include <climits>
//#include <combaseapi.h>
//#include <d3d12.h>
//#include <d3dcommon.h>
//#include <d3dx12_core.h>
//#include <d3dx12_default.h>
//#include <dxgiformat.h>
//#include <functional>
//#include <mutex>
//#include <string>
//#include <unordered_map>
//#include <Windows.h>
//#include <wrl\client.h>
//#include "Renderer.h"
//#include "Texture.h"
//
//// --- Thread-safe PSO cache setup ---
//struct PSOKey {
//	std::wstring vsPath;
//	std::wstring psPath;
//	const void* rootSignature;
//	DXGI_FORMAT rtvFormat;
//	DXGI_FORMAT dsvFormat;
//
//	bool operator==( const PSOKey& other ) const
//	{
//		return vsPath == other.vsPath &&
//			psPath == other.psPath &&
//			rootSignature == other.rootSignature &&
//			rtvFormat == other.rtvFormat &&
//			dsvFormat == other.dsvFormat;
//	}
//};
//
//namespace std {
//	template <>
//	struct hash<PSOKey> {
//		size_t operator()( const PSOKey& key ) const
//		{
//			size_t h1 = hash<wstring>()(key.vsPath);
//			size_t h2 = hash<wstring>()(key.psPath);
//			size_t h3 = hash<const void*>()(key.rootSignature);
//			size_t h4 = hash<int>()(static_cast<int>(key.rtvFormat));
//			size_t h5 = hash<int>()(static_cast<int>(key.dsvFormat));
//			return (((((h1 ^ (h2 << 1)) ^ (h3 << 1)) ^ (h4 << 1)) ^ (h5 << 1)));
//		}
//	};
//}
//
//static std::unordered_map<PSOKey, Microsoft::WRL::ComPtr<ID3D12PipelineState>> g_psoCache;
//static std::mutex g_psoCacheMutex;
//// -----------------------------------
//
//Material::Material( const std::wstring& vsPath, const std::wstring& psPath )
//	: vsPath( vsPath ), psPath( psPath )
//{
//	// Textures and fallbacks are initialized in the header
//}
//
//void Material::CreatePipelineState(
//	Renderer& renderer,
//	ID3D12RootSignature* rootSignature,
//	const D3D12_INPUT_LAYOUT_DESC& inputLayout,
//	DXGI_FORMAT rtvFormat,
//	DXGI_FORMAT dsvFormat )
//{
//	PSOKey key{ vsPath, psPath, rootSignature, rtvFormat, dsvFormat };
//
//	{
//		std::lock_guard<std::mutex> lock( g_psoCacheMutex );
//		auto it = g_psoCache.find( key );
//		if( it != g_psoCache.end() ) {
//			pipelineState = it->second;
//			return;
//		}
//	}
//
//	// Load shaders
//	vsBlob = renderer.LoadShaderBlob( vsPath.c_str() );
//	psBlob = renderer.LoadShaderBlob( psPath.c_str() );
//	assert( vsBlob && psBlob );
//
//	// Fill out the PSO description
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
//	psoDesc.pRootSignature = rootSignature;
//	psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
//	psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
//	psoDesc.InputLayout = inputLayout;
//	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC( D3D12_DEFAULT );
//	psoDesc.BlendState = CD3DX12_BLEND_DESC( D3D12_DEFAULT );
//	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC( D3D12_DEFAULT );
//	psoDesc.SampleMask = uint_MAX;
//	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//	psoDesc.NumRenderTargets = 1;
//	psoDesc.RTVFormats[0] = rtvFormat;
//	psoDesc.DSVFormat = dsvFormat;
//	psoDesc.SampleDesc.Count = 1;
//
//	auto device = renderer.GetDevice();
//	HRESULT hr = device->CreateGraphicsPipelineState( &psoDesc, IID_PPV_ARGS( &pipelineState ) );
//	assert( SUCCEEDED( hr ) );
//
//	{
//		std::lock_guard<std::mutex> lock( g_psoCacheMutex );
//		g_psoCache[key] = pipelineState;
//	}
//}
//
//void Material::Bind( ID3D12GraphicsCommandList* cmdList ) const
//{
//	// Bind the pipeline state
//	cmdList->SetPipelineState( pipelineState.Get() );
//
//	// Bind textures (SRVs) if present
//	// The root parameter indices must match your root signature!
//	// Example: 0 = albedo, 1 = normal, 2 = metallic, 3 = roughness, 4 = ao
//
//	if( albedo && albedo->IsReady() )
//		cmdList->SetGraphicsRootDescriptorTable( 0, albedo->GetSrvGpuHandle() );
//	// else: shader should use albedoFallback
//
//	if( normal && normal->IsReady() )
//		cmdList->SetGraphicsRootDescriptorTable( 1, normal->GetSrvGpuHandle() );
//	// else: shader should use normalFallback
//
//	if( metallic && metallic->IsReady() )
//		cmdList->SetGraphicsRootDescriptorTable( 2, metallic->GetSrvGpuHandle() );
//	// else: shader should use metallicFallback
//
//	if( roughness && roughness->IsReady() )
//		cmdList->SetGraphicsRootDescriptorTable( 3, roughness->GetSrvGpuHandle() );
//	// else: shader should use roughnessFallback
//
//	if( ao && ao->IsReady() )
//		cmdList->SetGraphicsRootDescriptorTable( 4, ao->GetSrvGpuHandle() );
//	// else: shader should use aoFallback
//
//	// Note: Fallback values should be set in a constant buffer and bound as well.
//	// This is typically done outside the material, or you can add a method to upload a material constant buffer.
//}

#include "Material.h"

void Material::Load( const std::string& path, GpuResourceManager& resourceManager )
{

}