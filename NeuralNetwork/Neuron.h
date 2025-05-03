#pragma once
#include <memory>
#include <vector>

class Synapse;
class NeuralCluster;

class Neuron
{
private:
	float potential;
	float activationThreshold;
	std::vector<Synapse> synapses;

public:
	void addPotential(const float& value);
	void fire(NeuralCluster* neuralCluster);
public:
	bool isActivated() const
	{
		return potential >= activationThreshold;
	}
};

