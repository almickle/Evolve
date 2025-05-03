#include "Utils.h"

float Utils::clamp(float value, float min, float max) {
	return std::max(min, std::min(value, max));
}

float Utils::randomFloat(float min, float max) {
	static std::random_device rd;
	static std::mt19937 rng(rd());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}