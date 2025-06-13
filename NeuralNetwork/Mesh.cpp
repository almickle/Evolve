#include <DirectXMath.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "DataStructures.h"
#include "GpuResourceManager.h"
#include "JsonSerializer.h"
#include "Mesh.h"
#include "SubMesh.h"
#include "Types.h"

std::string Mesh::Serialize( JsonSerializer& serializer ) const
{
	serializer.StartDocument();
	serializer.BeginObject();

	// Meta data and asset ids
	SerializeBaseAsset( serializer );

	// meshData
	serializer.BeginArray( "meshData" );
	for( const auto& mesh : meshData ) {
		serializer.BeginObject();

		// Vertices as array of objects
		serializer.BeginArray( "vertices" );
		for( const auto& vertex : mesh.vertices ) {
			serializer.BeginObject();
			// Serialize XMFLOAT3 position
			serializer.BeginArray( "position" );
			serializer.WriteValue( vertex.position.x );
			serializer.WriteValue( vertex.position.y );
			serializer.WriteValue( vertex.position.z );
			serializer.EndArray();

			// Serialize XMFLOAT3 normal
			serializer.BeginArray( "normal" );
			serializer.WriteValue( vertex.normal.x );
			serializer.WriteValue( vertex.normal.y );
			serializer.WriteValue( vertex.normal.z );
			serializer.EndArray();

			// Serialize XMFLOAT2 uv
			serializer.BeginArray( "uv" );
			serializer.WriteValue( vertex.uv.x );
			serializer.WriteValue( vertex.uv.y );
			serializer.EndArray();

			// Serialize XMFLOAT3 tangent
			serializer.BeginArray( "tangent" );
			serializer.WriteValue( vertex.tangent.x );
			serializer.WriteValue( vertex.tangent.y );
			serializer.WriteValue( vertex.tangent.z );
			serializer.WriteValue( vertex.tangent.w );
			serializer.EndArray();

			serializer.EndObject();
		}
		serializer.EndArray();

		// Indices
		serializer.WriteArray( "indices", mesh.indices );

		serializer.EndObject();
	}
	serializer.EndArray();

	serializer.EndObject();
	serializer.EndDocument();

	return serializer.GetString();
}

void Mesh::Load( GpuResourceManager& resourceManager, JsonSerializer& serializer )
{
	Deserialize( serializer );
	for( const auto& meshDatum : meshData )
	{
		auto vbId = resourceManager.CreateVertexBuffer( meshDatum.vertices, name );
		auto ibId = resourceManager.CreateIndexBuffer( meshDatum.indices, name );

		auto subMesh = std::make_unique<SubMesh>( vbId, ibId, name );
		AddSubAsset( std::move( subMesh ) );
	}
}

void Mesh::Deserialize( JsonSerializer& serializer )
{
	DeserializeBaseAsset( serializer );

	// meshData is an array of objects
	auto meshDataArray = serializer.GetSubObject( "meshData" );
	meshData.clear();
	for( const auto& meshJson : meshDataArray ) {
		MeshData md;
		for( const auto& vertexJson : meshJson.at( "vertices" ) ) {
			Vertex vertex{};
			// Deserialize XMFLOAT3 position
			auto& positionArray = vertexJson.at( "position" );
			vertex.position = DirectX::XMFLOAT3( positionArray[0].get<float>(), positionArray[1].get<float>(), positionArray[2].get<float>() );
			// Deserialize XMFLOAT3 normal
			auto&& normalArray = vertexJson.at( "normal" );
			vertex.normal = DirectX::XMFLOAT3( normalArray[0].get<float>(), normalArray[1].get<float>(), normalArray[2].get<float>() );
			// Deserialize XMFLOAT2 uv
			auto& uvArray = vertexJson.at( "uv" );
			vertex.uv = DirectX::XMFLOAT2( uvArray[0].get<float>(), uvArray[1].get<float>() );
			// Deserialize XMFLOAT3 tangent
			auto& tangentArray = vertexJson.at( "tangent" );
			vertex.tangent = DirectX::XMFLOAT4( tangentArray[0].get<float>(), tangentArray[1].get<float>(), tangentArray[2].get<float>(), tangentArray[3].get<float>() );
			md.vertices.push_back( vertex );
		}
		md.indices = meshJson.at( "indices" ).get<std::vector<uint>>();
		meshData.push_back( md );
	}
}