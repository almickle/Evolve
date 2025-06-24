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
	VectorMake,
	// Logic
	And,
	Or,
	GreaterThan,
	GreaterThanOrEqual,
	LessThan,
	LessThanOrEqual,
	Equal,
	NotEqual,
	// Arithmetic
	Add,
	Subtract,
	Multiply,
	Divide,
	Power,
	Radical,
	Negate,
	AbsoluteValue,
	Reciprocal,
	Sign,
	// Mesh
	Displacement,
	EmitTriangle,
	EmitVertex,
	ThreadInfo,
	MeshPrimitivePlane
};

enum class NodeDataType
{
	Int,
	Uint,
	Float,
	Bool,
	String,
	Uint3,
	Vector,
	Color,
};

enum class NodeParameterType {
	Texture,
	Vector,
	Scalar,
	Integer
};

class NodeSlot {
	using NodeValue = std::variant<
		int,
		unsigned int,
		bool,
		float,
		DirectX::XMUINT3,
		DirectX::XMFLOAT4
	>;
public:
	NodeSlot( const std::string& name, const NodeValue& data )
		: name( name ), data( data )
	{
		std::visit( [this]( auto&& arg ) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr( std::is_same_v<T, int> )
				dataType = NodeDataType::Int;
			else if constexpr( std::is_same_v<T, unsigned int> )
				dataType = NodeDataType::Uint;
			else if constexpr( std::is_same_v<T, DirectX::XMUINT3> )
				dataType = NodeDataType::Uint3;
			else if constexpr( std::is_same_v<T, bool> )
				dataType = NodeDataType::Bool;
			else if constexpr( std::is_same_v<T, float> )
				dataType = NodeDataType::Float;
			else if constexpr( std::is_same_v<T, DirectX::XMFLOAT4> )
				dataType = NodeDataType::Vector;
			else
				dataType = NodeDataType::Int;
					}, data );
	}
	NodeSlot( const std::string& name, const NodeValue& data, NodeDataType dataType )
		: name( name ), data( data ), dataType( dataType )
	{
	};
	NodeSlot() = default;
	~NodeSlot() = default;
public:
	std::string GetName() const { return name; }
	NodeDataType GetDataType() const { return dataType; }
	NodeValue GetData() const { return data; }
public:
	std::string name = "Default Slot";
	NodeValue data = 0;
	NodeDataType dataType = NodeDataType::Int;
public:
	std::string GetHlslSnippet() const
	{
		return std::visit( [this]( auto&& arg ) -> std::string {
			using T = std::decay_t<decltype(arg)>;
			if constexpr( std::is_same_v<T, int> )
				return "int " + name;
			else if constexpr( std::is_same_v<T, unsigned int> )
				return "uint " + name;
			else if constexpr( std::is_same_v<T, DirectX::XMUINT3> )
				return "uint3 " + name;
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
