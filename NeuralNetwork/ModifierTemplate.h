#pragma once
#include <d3dcommon.h>
#include <string>
#include <wrl/client.h>
#include "Asset.h"
#include "JsonSerializer.h"
#include "SystemManager.h"
#include "Types.h"

using namespace Microsoft::WRL;

class ModifierTemplate :
	public Asset
{
public:
	ModifierTemplate( const std::string& name = "ModifierTemplate" )
		: Asset( AssetType::ModifierTemplate, name )
	{
	}
public:
	void GenerateShaderCode();
	void Load( SystemManager* systemManager ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
public:
	std::string& GetShaderCode() { return shaderCode; }
	ComPtr<ID3DBlob>& GetVertexShaderBlob() { return vsBlob; };
	ComPtr<ID3DBlob>& GetDomainShaderBlob() { return dsBlob; };
	ComPtr<ID3DBlob>& GetHullShaderBlob() { return hsBlob; };
private:
	std::string shaderCode;
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> dsBlob;
	ComPtr<ID3DBlob> hsBlob;
};

