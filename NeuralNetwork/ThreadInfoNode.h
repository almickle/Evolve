#pragma once
#include <DirectXMath.h>
#include <format>
#include <string>
#include "NodeTypes.h"
#include "ShaderNode.h"
#include "Types.h"

class ThreadInfoNode : public ShaderNode {
public:
	ThreadInfoNode( const std::string& name = "ThreadInfoNode" )
		: ShaderNode( 0, 6, 0, name )
	{
		AddOutput( tidOutputSlot, NodeSlot{ "tid", DirectX::XMUINT3{} } );
		AddOutput( dtidOutputSlot, NodeSlot{ "dtid", DirectX::XMUINT3{} } );
		AddOutput( gidOutputSlot, NodeSlot{ "gid", DirectX::XMUINT3{} } );
		AddOutput( xThreadsOutputSlot, NodeSlot{ "threads_x", uint( 0 ) } );
		AddOutput( yThreadsOutputSlot, NodeSlot{ "threads_y", uint( 0 ) } );
		AddOutput( zThreadsOutputSlot, NodeSlot{ "threads_z", uint( 0 ) } );
	}
	~ThreadInfoNode() = default;
public:
	std::string GetShaderFunction() override
	{
		auto functionSignature = GetFunctionSignature();
		auto returnObject = GetReturnObject();
		auto returnStatement = GetReturnStatement();

		std::string functionBody = std::format(
			"output.{} = threadIds.tid;\n"
			"output.{} = threadIds.dtid;\n"
			"output.{} = threadIds.gid;\n"
			"output.{} = THREADS_X;\n"
			"output.{} = THREADS_Y;\n"
			"output.{} = THREADS_Z;\n",
			GetTidInput().name,
			GetDtidInput().name,
			GetGidInput().name,
			GetThreadsXInput().name,
			GetThreadsYInput().name,
			GeThreadsZInput().name );

		std::string shaderFunction = std::format( "{}{{\n{}\n{}\n{}}}", functionSignature, returnObject, functionBody, returnStatement );

		return shaderFunction;
	}
private:
	NodeSlot GetTidInput() const { return outputs[tidOutputSlot]; }
	NodeSlot GetDtidInput() const { return outputs[dtidOutputSlot]; }
	NodeSlot GetGidInput() const { return outputs[gidOutputSlot]; }
	NodeSlot GetThreadsXInput() const { return outputs[xThreadsOutputSlot]; }
	NodeSlot GetThreadsYInput() const { return outputs[yThreadsOutputSlot]; }
	NodeSlot GeThreadsZInput() const { return outputs[zThreadsOutputSlot]; }
private:
	uint tidOutputSlot = 0;
	uint dtidOutputSlot = 1;
	uint gidOutputSlot = 2;
	uint xThreadsOutputSlot = 3;
	uint yThreadsOutputSlot = 4;
	uint zThreadsOutputSlot = 5;
};