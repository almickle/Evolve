#include "NeuralCluster.h"
#include "Utils.h"

void NeuralCluster::compute()
{
	for (const auto& neuron : activeNeurons)
	{
		neuron->fire(this);
	}
}

void NeuralCluster::addToNextActiveNeurons(Neuron* neuron)
{
	nextActiveNeurons.push_back(neuron);
}
void NeuralCluster::step()
{
	compute();
	Utils::removeDuplicatePointers(nextActiveNeurons);
	activeNeurons.swap(nextActiveNeurons);
	nextActiveNeurons.clear();
}
std::vector<std::shared_ptr<Neuron>>& NeuralCluster::getNeuronList()
{
	return neurons;
}
std::shared_ptr<Neuron> NeuralCluster::getNeuronByIndex(const uint32_t& neuronIndex)
{
	return neurons[neuronIndex];
}