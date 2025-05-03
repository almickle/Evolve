#include "NeuralCluster.h"
#include "Neuron.h"
#include "Synapse.h"

void Synapse::fire(NeuralCluster* neuralCluster) const
{
	const auto& neuron = neuralCluster->getNeuronByIndex(neuronIndex);
	neuron->addPotential(strength);
}