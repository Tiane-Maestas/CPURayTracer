#pragma once
#include <random>

inline int random_int(std::mt19937& rng, int upperbound)
{
	std::uniform_int_distribution<int> dist(0, upperbound);
	return dist(rng);
}

inline float random_float(std::mt19937& rng)
{
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	return dist(rng);
}

inline double random_double(std::mt19937& rng)
{
	std::uniform_real_distribution<double> dist(0.0, 1.0);
	return dist(rng);
}
