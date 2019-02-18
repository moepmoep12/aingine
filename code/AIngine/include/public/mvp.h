#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

glm::mat4 CalculateMVP(float Translate, glm::vec2 const &Rotate);
