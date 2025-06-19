#pragma once
#include <DirectXMath.h>
#include <string>
#include <type_traits>
#include <variant>
#include "Types.h"

enum class NodeTypes
{
	MaterialOutput,
	TextureSampler,
	VertexData,
	VectorScale,
	NormalMap,
	VectorBreak,
	ScalarParameter,
	VectorMakeNode
};

enum class NodeParameterType {
	Texture,
	Vector,
	Scalar,
	Integer
};

struct NodeSlot {
	using NodeValue = std::variant<
		int,
		unsigned int,
		bool,
		float,
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
			else if constexpr( std::is_same_v<T, DirectX::XMFLOAT4> )
				return "float4 " + name;
			else
				return "/* unknown type */ " + name;
						   }, data );
	}
	std::string GetHlslValue() const
	{
		return std::visit( [this]( auto&& arg ) -> std::string {
			using T = std::decay_t<decltype(arg)>;
			if constexpr( std::is_same_v<T, int> )
				return std::to_string( arg );
			else if constexpr( std::is_same_v<T, unsigned int> )
				return std::to_string( arg );
			else if constexpr( std::is_same_v<T, bool> )
				return arg ? "true" : "false";
			else if constexpr( std::is_same_v<T, float> )
				return std::to_string( arg );
			else if constexpr( std::is_same_v<T, DirectX::XMFLOAT4> )
				return "float4(" + std::to_string( arg.x ) + ", " + std::to_string( arg.y ) + ", " + std::to_string( arg.z ) + ", " + std::to_string( arg.w ) + ")";
			else
				return "/* unknown type */ ";
						   }, data );
	}
};

struct NodeParameter {
	NodeParameterType type;
	std::string name;
	std::string GetHlslSnippet() const { return "uint " + name; }
};

struct NodeConnection {
	uint fromNode;      // Index source node
	uint fromSlot;		// Index of output slot
	uint toNode;        // Index destination node
	uint toSlot;		// Index of input slot
};

struct ParameterBinding {
	uint nodeIndex;
	uint parameterIndex;
	NodeParameterType parameterType;
	uint cbufferSlot;
};
