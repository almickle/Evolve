#pragma once
#include "Neuron.h"
#include <algorithm>
#include <memory>
#include <vector>

class NeuralCluster
{
private:
	std::vector<std::shared_ptr<Neuron>> neurons;
	std::vector<Neuron*> activeNeurons;
	std::vector<Neuron*> nextActiveNeurons;

public:
	void compute();
	void addToNextActiveNeurons(Neuron* neuron);
	void step();
public:
	std::vector<std::shared_ptr<Neuron>>& getNeuronList();
	std::shared_ptr<Neuron> getNeuronByIndex(const uint32_t& neuronIndex);
};

