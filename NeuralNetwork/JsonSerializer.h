#pragma once
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

class JsonSerializer {
public:
	JsonSerializer() = default;
	void StartDocument();
	void EndDocument();
	std::string GetString() const { return ss.str(); }
	void BeginObject();
	void EndObject();
	void BeginArray( const std::string& name );
	void EndArray();
	template<typename T>
	void Write( const std::string& name, const T& value );
	template<typename T>
	void WriteArray( const std::string& name, const std::vector<T>& arr );
public:
	void WriteValue( const std::string& value );
	void WriteValue( const char* value );
	void WriteValue( bool value );
	template<typename T>
	void WriteValue( const T& value );
public:
	void LoadFromFile( const std::string& path );
	template<typename T>
	T Read( const std::string& key ) const;
	template<typename T>
	std::vector<T> ReadArray( const std::string& key ) const;
	nlohmann::json GetSubObject( const std::string& key ) const;
private:
	void WriteName( const std::string& name );
	void WriteCommaIfNeeded();
	void MarkElementWritten();
	bool InArray() const;
private:
	enum class Context { Object, Array };
	std::ostringstream ss;
	std::vector<Context> contextStack;
	std::vector<bool> firstElementStack;
	nlohmann::json json;
};

template<typename T>
void JsonSerializer::WriteValue( const T& value )
{
	if constexpr( std::is_enum_v<T> ) {
		if( InArray() ) WriteCommaIfNeeded();
		ss << static_cast<std::underlying_type_t<T>>(value);
		MarkElementWritten();
	}
	else if constexpr( std::is_arithmetic_v<T> ) {
		if( InArray() ) WriteCommaIfNeeded();
		ss << value;
		MarkElementWritten();
	}
	else {
		static_assert(
			std::is_same_v<T, std::string> ||
			std::is_same_v<T, const char*> ||
			std::is_same_v<T, bool>,
			"JsonSerializer::WriteValue: Type is not supported for serialization. Add a WriteValue overload for this type."
			);
	}
}

template<typename T>
void JsonSerializer::Write( const std::string& name, const T& value )
{
	WriteCommaIfNeeded(); // Only here for objects
	WriteName( name );
	WriteValue( value );
	MarkElementWritten();
}

template<typename T>
void JsonSerializer::WriteArray( const std::string& name, const std::vector<T>& arr )
{
	WriteCommaIfNeeded();
	WriteName( name );
	ss << "[";
	contextStack.push_back( Context::Array );
	firstElementStack.push_back( true );
	for( const auto& v : arr ) {
		WriteValue( v );
	}
	contextStack.pop_back();
	firstElementStack.pop_back();
	ss << "]";
	MarkElementWritten();
}

// Example: get a value by key
template<typename T>
T JsonSerializer::Read( const std::string& key ) const
{
	return json.at( key ).get<T>();
}

// Example: get an array
template<typename T>
std::vector<T> JsonSerializer::ReadArray( const std::string& key ) const
{
	return json.at( key ).get<std::vector<T>>();
}