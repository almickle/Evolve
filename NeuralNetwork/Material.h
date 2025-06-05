//#pragma once
//#include <d3d12.h>
//#include <d3dcommon.h>
//#include <dxgiformat.h>
//#include <memory>
//#include <string>
//#include <wrl\client.h>
//
//class Renderer;
//class Texture;
//struct PSOKey;
//
//class Material {
//public:
//	Material( const std::wstring& vsPath, const std::wstring& psPath );
//
//	// PBR textures (optional)
//	std::shared_ptr<Texture> albedo;
//	std::shared_ptr<Texture> normal;
//	std::shared_ptr<Texture> metallic;
//	std::shared_ptr<Texture> roughness;
//	std::shared_ptr<Texture> ao;
//
//	// Fallbacks for PBR parameters
//	// Albedo fallback: RGBA color (default: white)
//	float albedoFallback[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
//	// Normal fallback: default normal (0,0,1)
//	float normalFallback[3] = { 0.0f, 0.0f, 1.0f };
//	// Metallic, roughness, AO fallback scalars
//	float metallicFallback = 0.0f;
//	float roughnessFallback = 1.0f;
//	float aoFallback = 1.0f;
//
//	// Pipeline state
//	void CreatePipelineState( Renderer& renderer,
//							  ID3D12RootSignature* rootSignature,
//							  const D3D12_INPUT_LAYOUT_DESC& inputLayout,
//							  DXGI_FORMAT rtvFormat,
//							  DXGI_FORMAT dsvFormat );
//
//	void Bind( ID3D12GraphicsCommandList* cmdList ) const;
//
//	ID3D12PipelineState* GetPipelineState() const { return pipelineState.Get(); }
//	const std::wstring& GetVSPath() const { return vsPath; }
//	const std::wstring& GetPSPath() const { return psPath; }
//
//private:
//	std::wstring vsPath;
//	std::wstring psPath;
//	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
//	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
//	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
//};