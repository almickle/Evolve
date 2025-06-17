#include <exception>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "JsonSerializer.h"
#include "ModifierTemplate.h"
#include "SystemManager.h"

void ModifierTemplate::GenerateShaderCode()
{
	// Load the contents of PbrVS.hlsl into shaderCode
	std::ifstream file( "PbrVS.hlsl" );
	if( !file ) {
		shaderCode.clear();
		return;
	}
	std::ostringstream ss;
	ss << file.rdbuf();
	shaderCode = ss.str();
}

void ModifierTemplate::Load( SystemManager* systemManager, JsonSerializer& serializer )
{
	Deserialize( serializer );
}

std::string ModifierTemplate::Serialize( JsonSerializer& serializer ) const
{
	serializer.StartDocument();
	SerializeBaseAsset( serializer );
	serializer.EndDocument();
	return serializer.GetString();
}

void ModifierTemplate::Deserialize( JsonSerializer& serializer )
{
	try
	{
		DeserializeBaseAsset( serializer );
	}
	catch( const std::exception& )
	{
		throw std::runtime_error( "Failed to deserialize ModifierTemplate" );
	}
}