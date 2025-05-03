#include "NeuralCluster.h"
#include "Neuron.h"
#include "Synapse.h"

void Neuron::addPotential(const float& value)
{
	potential += value;
}
void Neuron::fire(NeuralCluster* neuralCluster)
{
	for (const auto& synapse : synapses)
	{
		synapse.fire(neuralCluster);
	}
	potential = 0.0f;
}