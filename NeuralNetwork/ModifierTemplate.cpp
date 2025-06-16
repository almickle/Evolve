#include <fstream>
#include <sstream>
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

void ModifierTemplate::Load( SystemManager* systemManager )
{
	Deserialize( *systemManager->GetSerializer() );
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
	DeserializeBaseAsset( serializer );
}