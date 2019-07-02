#include "Util/Random.h"

#include <time.h>
#include <random>

namespace AIngine::Util::Random {

	static bool s_init = false;

	int RandomInt(int min, int max)
	{
		if (!s_init) {
			srand(time(NULL));
			s_init = true;
		}

		return rand() % max + min;
	}

	float RandomFloat(float min, float max)
	{
		if (!s_init) {
			srand(time(NULL));
			s_init = true;
		}

		return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	}
}