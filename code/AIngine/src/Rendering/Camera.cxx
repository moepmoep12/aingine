#include "Rendering/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Rendering/Viewport.h"
#include <algorithm>

AIngine::Rendering::Camera::Camera(const Viewport & viewport, const glm::vec2& bounds, const glm::vec2& desiredScreenSizeInWorldUnits)
	: m_viewport(viewport)
{
	m_origin = glm::vec2(viewport.GetViewportWidth() / 2.0, viewport.GetViewportHeight() / 2.0);
	glm::vec2 screenSize;

	if (desiredScreenSizeInWorldUnits == glm::vec2(0))
	{
		screenSize = bounds;
	}
	else {
		screenSize = desiredScreenSizeInWorldUnits;
	}

	m_zoom = viewport.GetWindowWidth() / screenSize.x;
}

void AIngine::Rendering::Camera::Zoom(float amount)
{
	m_zoom = std::clamp(m_zoom + amount, m_zoomMin, m_zoomMax);
}

void AIngine::Rendering::Camera::Rotate(float amount)
{
	m_rotation += amount;
	m_rotation = std::fmod(m_rotation, M_PI);
}

void AIngine::Rendering::Camera::LookAt(const glm::vec2& pos)
{
	m_position = WorldToScreenPoint(pos) - glm::vec2(m_viewport.GetViewportWidth() / 2.0, m_viewport.GetViewportHeight() / 2.0);
}

glm::mat4  AIngine::Rendering::Camera::GetViewMatrix() const
{
	return GetViewMatrix(glm::vec2(1.0));
}

glm::mat4 AIngine::Rendering::Camera::GetViewMatrix(const glm::vec2 & parallaxFactor) const
{
	return GetVirtualViewMatrix(parallaxFactor) * m_viewport.GetScaleMat();
}

glm::vec2 AIngine::Rendering::Camera::ScreenToWorldPoint(const glm::vec2 & screenpoint) const
{
	glm::vec2 point = screenpoint - m_viewport.GetTopLeftCornerPosition();
	glm::mat4 inverseView = glm::inverse(GetViewMatrix());
	return inverseView * glm::vec4(point, 0, 1);
}

glm::vec2 AIngine::Rendering::Camera::WorldToScreenPoint(const glm::vec2 & worldpoint) const
{
	glm::vec2 point = worldpoint /*+ m_viewport.GetTopLeftCornerPosition()*/;
	return  GetViewMatrix() * glm::vec4(point, 0, 1);
}


glm::mat4 AIngine::Rendering::Camera::GetVirtualViewMatrix(const glm::vec2 & parallaxFactor) const
{
	glm::mat4 mat = glm::mat4(1.0);
	mat = glm::translate(mat, glm::vec3(-m_position * parallaxFactor, 0.0));
	//mat = glm::translate(mat, glm::vec3(-m_origin, 0.0));
	mat = glm::rotate(mat, m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	mat = glm::scale(mat, glm::vec3(m_zoom, m_zoom, 1.0));
	//mat = glm::translate(mat, glm::vec3(m_origin, 0.0));

	return mat;
}
