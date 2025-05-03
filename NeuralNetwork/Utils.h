#pragma once
#include <algorithm>
#include <random>

class Neuron;

namespace Utils {
	float clamp(float value, float min, float max);
	float randomFloat(float min, float max);
	template<typename T>
	void removeDuplicatePointers(std::vector<T*>& vec) {
		std::sort(vec.begin(), vec.end());
		auto last = std::unique(vec.begin(), vec.end());
		vec.erase(last, vec.end());
	}
}