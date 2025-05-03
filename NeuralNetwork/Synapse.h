#pragma once
#include <memory>
#include <stdint.h>
#include <vector>

class Neuron;
class NeuralCluster;

class Synapse
{
private:
	uint32_t neuronIndex;
	float strength;

public:
	void fire(NeuralCluster* neuralCluster) const;
};

