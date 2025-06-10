#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstdint>
#include <DirectXMath.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "DataStructures.h"
#include "GpuResourceManager.h"
#include "Mesh.h"
#include "SubMesh.h"

void Mesh::Load( const std::string& path, GpuResourceManager& manager )
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

	for( unsigned int m = 0; m < scene->mNumMeshes; ++m ) {
		const aiMesh* mesh = scene->mMeshes[m];
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		vertices.reserve( mesh->mNumVertices );
		for( unsigned int i = 0; i < mesh->mNumVertices; ++i ) {
			Vertex v{};
			v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			v.normal = mesh->HasNormals() ? DirectX::XMFLOAT3( mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z ) : DirectX::XMFLOAT3( 0, 0, 0 );
			if( mesh->HasTextureCoords( 0 ) )
				v.texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			else
				v.texcoord = { 0, 0 };
			if( mesh->HasTangentsAndBitangents() )
				v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
			else
				v.tangent = { 0, 0, 0 };
			vertices.push_back( v );
		}

		indices.reserve( static_cast<std::vector<uint32_t, std::allocator<uint32_t>>::size_type>(mesh->mNumFaces) * 3 );
		for( unsigned int i = 0; i < mesh->mNumFaces; ++i ) {
			const aiFace& face = mesh->mFaces[i];
			for( unsigned int j = 0; j < face.mNumIndices; ++j )
				indices.push_back( face.mIndices[j] );
		}

		auto vbId = manager.CreateVertexBuffer( vertices, debugName );
		auto ibId = manager.CreateIndexBuffer( indices, debugName );

		auto subMesh = std::make_unique<SubMesh>( vbId, ibId, debugName );
		AddSubAsset( std::move( subMesh ) );
	}
}