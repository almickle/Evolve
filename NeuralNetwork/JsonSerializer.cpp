#include <fstream>
#include <iomanip>
#include <nlohmann\json_fwd.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "JsonSerializer.h"

void JsonSerializer::StartDocument()
{
	ss.str( "" );
	ss.clear();
	contextStack.clear();
	firstElementStack.clear();
}
void JsonSerializer::EndDocument()
{
	if( !contextStack.empty() ) {
		throw std::runtime_error( "JsonSerializer: Document ended with unclosed context." );
	}
}
void JsonSerializer::BeginObject()
{
	WriteCommaIfNeeded();
	ss << "{";
	contextStack.push_back( Context::Object );
	firstElementStack.push_back( true );
}

void JsonSerializer::EndObject()
{
	ss << "}";
	contextStack.pop_back();
	firstElementStack.pop_back();
	MarkElementWritten();
}

void JsonSerializer::BeginArray( const std::string& name )
{
	WriteCommaIfNeeded();
	WriteName( name );
	ss << "[";
	contextStack.push_back( Context::Array );
	firstElementStack.push_back( true );
}

void JsonSerializer::EndArray()
{
	ss << "]";
	contextStack.pop_back();
	firstElementStack.pop_back();
	MarkElementWritten();
}

// Value writers
void JsonSerializer::WriteValue( const std::string& value )
{
	if( InArray() ) WriteCommaIfNeeded();
	ss << std::quoted( value );
	MarkElementWritten();
}
void JsonSerializer::WriteValue( const char* value )
{
	if( InArray() ) WriteCommaIfNeeded();
	ss << std::quoted( std::string( value ) );
	MarkElementWritten();
}
void JsonSerializer::WriteValue( bool value )
{
	if( InArray() ) WriteCommaIfNeeded();
	ss << (value ? "true" : "false");
	MarkElementWritten();
}

void JsonSerializer::WriteName( const std::string& name )
{
	ss << "\n  " << std::quoted( name ) << ": ";
}

void JsonSerializer::WriteCommaIfNeeded()
{
	if( !firstElementStack.empty() && !firstElementStack.back() ) {
		ss << ",";
	}
}

void JsonSerializer::MarkElementWritten()
{
	if( !firstElementStack.empty() ) {
		firstElementStack.back() = false;
	}
}

bool JsonSerializer::InArray() const
{
	return !contextStack.empty() && contextStack.back() == Context::Array;
}

// Deserialization
void JsonSerializer::LoadFromFile( const std::string& path )
{
	std::ifstream file( path );
	if( !file.is_open() ) {
		throw std::runtime_error( "JsonSerializer: Failed to open file: " + path );
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	json = nlohmann::json::parse( buffer.str() );
}

// For nested objects, you can return a sub-json
nlohmann::json JsonSerializer::GetSubObject( const std::string& key ) const
{
	return json.at( key );
}