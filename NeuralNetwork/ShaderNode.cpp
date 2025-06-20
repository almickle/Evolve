#include <sstream>
#include <string>
#include "NodeTypes.h"
#include "ShaderNode.h"
#include "Types.h"

std::string ShaderNode::GetInputStatement( const uint& nodeIndex ) const
{
	return GetInputStructName() + " " + GetInputDataName( nodeIndex ) + ";";
}

std::string ShaderNode::GetOutputStatement( const uint& nodeIndex ) const
{
	return GetOutputStructName() + " " + GetOutputDataName( nodeIndex ) + " = " + GetFunctionName() + "(" + GetInputDataName( nodeIndex ) + ", " + GetParameterDataName( nodeIndex ) + ", " + "vertexData" + ");";
}
std::string ShaderNode::GetParameterStatement( const uint& nodeIndex ) const
{
	return GetParameterStructName() + " " + GetParameterDataName( nodeIndex ) + ";";
}

std::string ShaderNode::GetInputSlotName( const uint& slot ) const
{
	return inputs[slot].name;
}

std::string ShaderNode::GetOutputSlotName( const uint& slot ) const
{
	return outputs[slot].name;
}

std::string ShaderNode::GetParameterSlotName( const uint& slot ) const
{
	return parameters[slot].name;
}

std::string ShaderNode::GetInputStruct() const
{
	std::ostringstream oss;
	oss << "struct " << GetInputStructName() << "{\n";
	for( const auto& input : inputs ) {
		oss << "    " << input.GetHlslSnippet() << ";\n";
	}
	oss << "};";
	return oss.str();
}

std::string ShaderNode::GetOutputStruct() const
{
	std::ostringstream oss;
	oss << "struct " << GetOutputStructName() << "{\n";
	for( const auto& output : outputs ) {
		oss << "    " << output.GetHlslSnippet() << ";\n";
	}
	oss << "};";
	return oss.str();
}

std::string ShaderNode::GetParameterStruct() const
{
	std::ostringstream oss;
	oss << "struct " << GetParameterStructName() << "{\n";
	for( const auto& param : parameters ) {
		oss << "    " << param.GetHlslSnippet() << ";\n";
	}
	oss << "};";
	return oss.str();
}

std::string ShaderNode::GetFunctionName() const
{
	return name;
}

std::string ShaderNode::GetInputStructName() const
{
	return name + "Input";
}

std::string ShaderNode::GetOutputStructName() const
{
	return name + "Output";
}

std::string ShaderNode::GetParameterStructName() const
{
	return name + "Parameters";
}


std::string ShaderNode::GetInputDataName( const uint& nodeIndex ) const
{
	return name + "InputData" + std::to_string( nodeIndex );
}

std::string ShaderNode::GetOutputDataName( const uint& nodeIndex ) const
{
	return name + "OutputData" + std::to_string( nodeIndex );
}

std::string ShaderNode::GetParameterDataName( const uint& nodeIndex ) const
{
	return name + "ParameterData" + std::to_string( nodeIndex );
}

std::string ShaderNode::GetReturnObject() const
{
	std::ostringstream oss;
	oss << name << "Output output;";
	return oss.str();
}

std::string ShaderNode::GetFunctionSignature() const
{
	auto returnType = outputs.size() > 0 ? GetOutputStructName() : "void";
	return returnType + " " + GetFunctionName() + "(" + GetInputStructName() + " input" + ", " + GetParameterStructName() + " parameters" + ", " + "VSOutput vertexData" + ")";
}

std::string ShaderNode::GetReturnStatement() const
{
	return "return output;";
}