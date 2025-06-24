#pragma once
#include <string>
#include "NodeLibrary.h"
#include "ShaderGraph.h"
#include "Types.h"

class MeshShader :
	public ShaderGraph
{
public:
	MeshShader( NodeLibrary& nodeLibrary, const std::string& name = "Mesh Shader" )
		: ShaderGraph( nodeLibrary, AssetType::MeshShader, ShaderType::Mesh, name )
	{

	}
	~MeshShader() = default;
};