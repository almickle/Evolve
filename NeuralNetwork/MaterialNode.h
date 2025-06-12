#pragma once
#include <string>
#include <vector>
#include "NodeTypes.h"
#include "Types.h"

class MaterialNode {
public:
	MaterialNode( uint inputCount, uint outputCount, uint parameterCount, const std::string& name = "MaterialNode" )
		: name( name )
	{
		inputs.resize( inputCount );
		outputs.resize( outputCount );
		parameters.resize( parameterCount );
	}
	virtual ~MaterialNode() = default;
public:
	virtual std::string GetShaderFunction() = 0;
	std::string GetInputStruct() const;
	std::string GetOutputStruct() const;
	std::string GetParameterStruct() const;
	std::string GetInputStatement( uint& nodeIndex ) const;
	std::string GetOutputStatement( uint& nodeIndex ) const;
	std::string GetParameterStatement( uint& nodeIndex ) const;
	std::string GetInputDataName( const uint& nodeIndex ) const;
	std::string GetOutputDataName( const uint& nodeIndex ) const;
	std::string GetParameterDataName( const uint& nodeIndex ) const;
	std::string GetInputSlotName( uint slot ) const;
	std::string GetOutputSlotName( uint slot ) const;
	std::string GetParameterSlotName( uint slot ) const;
	uint GetParameterCount() const { return parameters.size(); };
public:
	const std::vector<NodeSlot>& GetInputs() const { return inputs; }
	const std::vector<NodeSlot>& GetOutputs() const { return outputs; }
	const std::vector<NodeParameter>& GetParameters() const { return parameters; }
	const NodeSlot& GetInput( const uint& slot ) const { return inputs[slot]; }
	const NodeSlot& GetOutput( const uint& slot ) const { return outputs[slot]; }
	const NodeParameter& GetParameter( const uint& slot ) const { return parameters[slot]; }
public:
	void AddInput( const uint& slot, const NodeSlot& input ) { inputs[slot] = input; }
	void AddOutput( const uint& slot, const NodeSlot& output ) { outputs[slot] = output; }
	void AddParameter( const uint& slot, NodeParameter parameter ) { parameters[slot] = parameter; }
protected:
	std::string GetFunctionName() const;
	//std::string GetParameterName( const uint& index ) const;
	std::string GetInputStructName() const;
	std::string GetOutputStructName() const;
	std::string GetParameterStructName() const;
	std::string GetFunctionSignature() const;
	std::string GetReturnObject() const;
	std::string GetReturnStatement() const;
protected:
	std::string name;
	std::vector<NodeSlot> inputs;
	std::vector<NodeSlot> outputs;
	std::vector<NodeParameter> parameters;
};