#include "Rendering/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

void AIngine::Rendering::Camera::Rotate(float amount)
{
	if()
}

const glm::mat4 & AIngine::Rendering::Camera::GetTransform() const
{
	glm::mat4 mat = glm::mat4(1.0);
	mat = glm::translate(mat, glm::vec3(m_position, 0.0));
	mat = glm::rotate(mat, m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	return mat;
}
