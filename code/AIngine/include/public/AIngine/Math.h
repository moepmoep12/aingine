#pragma once
#include "Constants.h"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/spline.hpp>

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

	inline glm::vec2 catmull_rom_spline(const std::vector<glm::vec2>& cp, float t)
	{
		// indices of the relevant control points
		int i0 = glm::clamp<int>(t - 1, 0, cp.size() - 1);
		int i1 = glm::clamp<int>(t, 0, cp.size() - 1);
		int i2 = glm::clamp<int>(t + 1, 0, cp.size() - 1);
		int i3 = glm::clamp<int>(t + 2, 0, cp.size() - 1);

		// parameter on the local curve interval
		float local_t = glm::fract(t);

		return glm::catmullRom(cp[i0], cp[i1], cp[i2], cp[i3], local_t);
	}

}