#include <string>
#include <vector>
#include "Asset.h"
#include "JsonSerializer.h"
#include "Material.h"

std::string Material::Serialize( JsonSerializer& serializer ) const
{
	serializer.StartDocument();
	serializer.BeginObject();

	// Meta data and asset ids
	SerializeBaseAsset( serializer );

	// Material template ID
	serializer.Write( "materialTemplateId", materialTemplate );

	// Texture bindings
	serializer.BeginArray( "textureBindings" );
	for( const auto& t : textureBindings ) {
		serializer.BeginObject();
		serializer.Write( "slot", t.slot );
		serializer.Write( "data", t.data );
		serializer.EndObject();
	}
	serializer.EndArray();

	// Vector bindings
	serializer.BeginArray( "vectorBindings" );
	for( const auto& v : vectorBindings ) {
		serializer.BeginObject();
		serializer.Write( "slot", v.slot );
		std::vector<float> vec = { v.data.x, v.data.y, v.data.z };
		serializer.WriteArray( "data", vec );
		serializer.EndObject();
	}
	serializer.EndArray();

	// Scalar bindings
	serializer.BeginArray( "scalarBindings" );
	for( const auto& s : scalarBindings ) {
		serializer.BeginObject();
		serializer.Write( "slot", s.slot );
		serializer.Write( "data", s.data );
		serializer.EndObject();
	}
	serializer.EndArray();

	serializer.EndObject();
	serializer.EndDocument();

	return serializer.GetString();
}

void Material::Load( GpuResourceManager& resourceManager, JsonSerializer& serializer )
{
	Deserialize( serializer );
}

void Material::Deserialize( JsonSerializer& serializer )
{
	// Base asset fields (id, name, type, assetIds, etc.)
	DeserializeBaseAsset( serializer );

	// Material template ID
	materialTemplate = serializer.Read<decltype(materialTemplate)>( "materialTemplateId" );

	// Texture bindings
	textureBindings.clear();
	auto texArray = serializer.GetSubObject( "textureBindings" );
	for( const auto& t : texArray ) {
		TextureBinding binding;
		binding.slot = t.at( "slot" ).get<decltype(binding.slot)>();
		binding.data = t.at( "data" ).get<decltype(binding.data)>();
		textureBindings.push_back( binding );
	}

	// Vector bindings
	vectorBindings.clear();
	auto vecArray = serializer.GetSubObject( "vectorBindings" );
	for( const auto& v : vecArray ) {
		VectorBinding binding{};
		binding.slot = v.at( "slot" ).get<decltype(binding.slot)>();
		auto& arr = v.at( "data" );
		binding.data.x = arr[0].get<float>();
		binding.data.y = arr[1].get<float>();
		binding.data.z = arr[2].get<float>();
		vectorBindings.push_back( binding );
	}

	// Scalar bindings
	scalarBindings.clear();
	auto scalarArray = serializer.GetSubObject( "scalarBindings" );
	for( const auto& s : scalarArray ) {
		ScalarBinding binding{};
		binding.slot = s.at( "slot" ).get<decltype(binding.slot)>();
		binding.data = s.at( "data" ).get<decltype(binding.data)>();
		scalarBindings.push_back( binding );
	}
}