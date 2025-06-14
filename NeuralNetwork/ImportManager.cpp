#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cctype>
#include <cstdint>
#include <DirectXMath.h>
#include <DirectXMathConvert.inl>
#include <DirectXMathVector.inl>
#include <DirectXTex.h>
#include <DirectXTex.inl>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <Windows.h>
#include "DataStructures.h"
#include "ImportManager.h"
#include "Mesh.h"

namespace {
	enum class ImageType {
		DDS, TGA, HDR, WIC, UNKNOWN
	};

	ImageType GetImageTypeFromExtension( const std::wstring& filePath )
	{
		auto pos = filePath.find_last_of( L'.' );
		if( pos == std::wstring::npos ) return ImageType::UNKNOWN;
		std::wstring ext = filePath.substr( pos + 1 );
		std::transform( ext.begin(), ext.end(), ext.begin(), ::towlower );

		if( ext == L"dds" ) return ImageType::DDS;
		if( ext == L"tga" ) return ImageType::TGA;
		if( ext == L"hdr" ) return ImageType::HDR;
		// Add more as needed
		return ImageType::WIC; // Default to WIC for common formats
	}
}

std::vector<MeshData> ImportManager::LoadMesh( const std::string& path )
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( path,
											  aiProcess_Triangulate |
											  aiProcess_GenSmoothNormals |
											  aiProcess_CalcTangentSpace |
											  aiProcess_JoinIdenticalVertices );

	if( !scene || !scene->HasMeshes() ) {
		throw std::runtime_error( "Failed to load model: " + path + " " + importer.GetErrorString() );
	}

	std::vector<MeshData> meshData;
	meshData.resize( scene->mNumMeshes );
	for( unsigned int m = 0; m < scene->mNumMeshes; ++m ) {
		const aiMesh* mesh = scene->mMeshes[m];
		std::vector<Vertex>& vertices = meshData[m].vertices;
		std::vector<uint32_t>& indices = meshData[m].indices;

		vertices.reserve( mesh->mNumVertices );
		for( unsigned int i = 0; i < mesh->mNumVertices; ++i ) {
			Vertex v{};
			v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			v.normal = mesh->HasNormals() ? DirectX::XMFLOAT3( mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z ) : DirectX::XMFLOAT3( 0, 0, 0 );
			if( mesh->HasTextureCoords( 0 ) )
				v.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			else
				v.uv = { 0, 0 };
			if( mesh->HasTangentsAndBitangents() ) {
				auto T = DirectX::XMFLOAT3{ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				auto B = DirectX::XMFLOAT3{ mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
				auto& N = v.normal;
				// Calculate handedness
				auto BN = DirectX::XMVector3Cross( DirectX::XMLoadFloat3( &N ), DirectX::XMLoadFloat3( &T ) );
				float w = (DirectX::XMVector3Dot( BN, DirectX::XMLoadFloat3( &B ) ).m128_f32[0] < 0.0f) ? -1.0f : 1.0f;

				v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, w };
			}
			else
				v.tangent = { 0, 0, 0, 1 };
			vertices.push_back( v );
		}

		indices.reserve( static_cast<std::vector<uint32_t, std::allocator<uint32_t>>::size_type>(mesh->mNumFaces) * 3 );
		for( unsigned int i = 0; i < mesh->mNumFaces; ++i ) {
			const aiFace& face = mesh->mFaces[i];
			for( unsigned int j = 0; j < face.mNumIndices; ++j )
				indices.push_back( face.mIndices[j] );
		}
	}

	return meshData;
}

std::shared_ptr<DirectX::ScratchImage> ImportManager::LoadTexture( const std::string& path )
{
	auto image = std::make_shared<DirectX::ScratchImage>();
	HRESULT hr = S_OK;
	std::wstring filePath( path.begin(), path.end() );

	ImageType type = GetImageTypeFromExtension( filePath );

	switch( type ) {
		case ImageType::DDS:
			hr = DirectX::LoadFromDDSFile(
				filePath.c_str(),
				DirectX::DDS_FLAGS_NONE,
				nullptr,
				*image
			);
			break;
		case ImageType::TGA:
			hr = DirectX::LoadFromTGAFile(
				filePath.c_str(),
				nullptr,
				*image
			);
			break;
		case ImageType::HDR:
			hr = DirectX::LoadFromHDRFile(
				filePath.c_str(),
				nullptr,
				*image
			);
			break;
		case ImageType::WIC:
		default:
			hr = DirectX::LoadFromWICFile(
				filePath.c_str(),
				DirectX::WIC_FLAGS_NONE,
				nullptr,
				*image
			);
			break;
	}
	return image;
}
