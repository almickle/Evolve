#include <sstream>
#include <string>
#include "MaterialNode.h"
#include "NodeTypes.h"
#include "Types.h"

std::string MaterialNode::GetInputStatement( uint& nodeIndex ) const
{
	return GetInputStructName() + " " + GetInputDataName( nodeIndex ) + ";";
}

std::string MaterialNode::GetOutputStatement( uint& nodeIndex ) const
{
	return GetOutputStructName() + " " + GetOutputDataName( nodeIndex ) + " = " + GetFunctionName() + "(" + GetInputDataName( nodeIndex ) + ", " + GetParameterDataName( nodeIndex ) + ");";
}
std::string MaterialNode::GetParameterStatement( uint& nodeIndex ) const
{
	return GetParameterStructName() + " " + GetParameterDataName( nodeIndex ) + ";";

}

std::string MaterialNode::GetInputSlotName( uint slot ) const
{
	return inputs[slot].name;
}

std::string MaterialNode::GetOutputSlotName( uint slot ) const
{
	return outputs[slot].name;
}

std::string MaterialNode::GetParameterSlotName( uint slot ) const
{
	return parameters[slot].name;
}

std::string MaterialNode::GetInputStruct() const
{
	std::ostringstream oss;
	oss << "struct " << GetInputStructName() << "{\n";
	for( const auto& input : inputs ) {
		oss << "    " << input.GetHlslSnippet() << ";\n";
	}
	oss << "};";
	return oss.str();
}

std::string MaterialNode::GetOutputStruct() const
{
	std::ostringstream oss;
	oss << "struct " << GetOutputStructName() << "{\n";
	for( const auto& output : outputs ) {
		oss << "    " << output.GetHlslSnippet() << ";\n";
	}
	oss << "};";
	return oss.str();
}

std::string MaterialNode::GetParameterStruct() const
{
	std::ostringstream oss;
	oss << "struct " << GetParameterStructName() << "{\n";
	for( const auto& param : parameters ) {
		oss << "    " << param.GetHlslSnippet() << ";\n";
	}
	oss << "};";
	return oss.str();
}

std::string MaterialNode::GetFunctionName() const
{
	return name;
}

std::string MaterialNode::GetInputStructName() const
{
	return name + "Input";
}

std::string MaterialNode::GetOutputStructName() const
{
	return name + "Output";
}

std::string MaterialNode::GetParameterStructName() const
{
	return name + "Parameters";
}


std::string MaterialNode::GetInputDataName( const uint& nodeIndex ) const
{
	return name + "InputData" + std::to_string( nodeIndex );
}

std::string MaterialNode::GetOutputDataName( const uint& nodeIndex ) const
{
	return name + "OutputData" + std::to_string( nodeIndex );
}

std::string MaterialNode::GetParameterDataName( const uint& nodeIndex ) const
{
	return name + "ParameterData" + std::to_string( nodeIndex );
}

std::string MaterialNode::GetReturnObject() const
{
	std::ostringstream oss;
	oss << name << "Output output;";
	return oss.str();
}

std::string MaterialNode::GetFunctionSignature() const
{
	return GetOutputStructName() + " " + GetFunctionName() + "(" + GetInputStructName() + " input" + ", " + GetParameterStructName() + " parameters" + ")";
}

std::string MaterialNode::GetReturnStatement() const
{
	return "return output;";
}