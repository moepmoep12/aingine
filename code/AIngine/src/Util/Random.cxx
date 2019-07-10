#include "Util/Random.h"

#include <time.h>
#include <random>

namespace AIngine::Util::Random {

	int RandomInt(int min, int max)
	{
		static std::random_device rd;
		std::uniform_int_distribution<int> uni(min, max);
		return uni(std::mt19937(rd()));
	}

	float RandomFloat(float min, float max)
	{
		static std::random_device rd;
		std::uniform_real_distribution<float> uni(min, max);
		return uni(std::mt19937(rd()));
	}
}