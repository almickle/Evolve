#pragma once
#include <DirectXMath.h>
#include <string>
#include <type_traits>
#include <variant>

enum class NodeTypes
{
	TextureSampler,
	MaterialOutput
};

enum class NodeParameterTypes {
	Texture,
	Vector,
	Scalar,
};

struct NodeSlot {
	using NodeValue = std::variant<
		int,
		unsigned int,
		bool,
		float,
		DirectX::XMFLOAT2,
		DirectX::XMFLOAT3,
		DirectX::XMFLOAT4
	>;
	std::string name;
	NodeValue data;
public:
	std::string GetHlslSnippet() const
	{
		return std::visit( [this]( auto&& arg ) -> std::string {
			using T = std::decay_t<decltype(arg)>;
			if constexpr( std::is_same_v<T, int> )
				return "int " + name;
			else if constexpr( std::is_same_v<T, unsigned int> )
				return "uint " + name;
			else if constexpr( std::is_same_v<T, bool> )
				return "bool " + name;
			else if constexpr( std::is_same_v<T, float> )
				return "float " + name;
			else if constexpr( std::is_same_v<T, DirectX::XMFLOAT2> )
				return "float2 " + name;
			else if constexpr( std::is_same_v<T, DirectX::XMFLOAT3> )
				return "float3 " + name;
			else if constexpr( std::is_same_v<T, DirectX::XMFLOAT4> )
				return "float4 " + name;
			else
				return "/* unknown type */ " + name;
						   }, data );
	}
};

struct NodeParameter {
	NodeParameterTypes type;
	std::string name;
	std::string GetHlslSnippet() const { return "uint " + name; }
};
