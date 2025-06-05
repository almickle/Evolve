#pragma once
#include <d3d12.h>
#include <d3dcommon.h>
#include <dxgiformat.h>
#include <memory>
#include <string>
#include <wrl\client.h>

class Renderer;
class Texture;
struct PSOKey;

class Material {
public:
	Material( const std::wstring& vsPath, const std::wstring& psPath );
	~Material() = default;
public:
	void CreatePipelineState( Renderer& renderer,
							  ID3D12RootSignature* rootSignature,
							  const D3D12_INPUT_LAYOUT_DESC& inputLayout,
							  DXGI_FORMAT rtvFormat,
							  DXGI_FORMAT dsvFormat );

	void Bind( ID3D12GraphicsCommandList* cmdList ) const;
public:
	ID3D12PipelineState* GetPipelineState() const { return pipelineState.Get(); }
	const std::wstring& GetVSPath() const { return vsPath; }
	const std::wstring& GetPSPath() const { return psPath; }
private:
	std::shared_ptr<Texture> albedo;
	std::shared_ptr<Texture> normal;
	std::shared_ptr<Texture> metallic;
	std::shared_ptr<Texture> roughness;
	std::shared_ptr<Texture> ao;
private:
	std::wstring vsPath;
	std::wstring psPath;
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
};