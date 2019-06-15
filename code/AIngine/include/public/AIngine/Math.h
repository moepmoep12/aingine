#pragma once
#include "Constants.h"
#include <glm/glm.hpp>

namespace AIngine::Math {

	inline float SinErp(float t) {
		return glm::sin(t * M_PI * 0.5f);
	}

	inline float CosErp(float t) {
		return glm::cos(t * M_PI * 0.5f);
	}

	inline float SmoothStep(float t) {
		return  t * t*t * (t * (6.0f*t - 15.0f) + 10.0f);
	}
}