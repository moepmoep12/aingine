#include "Rendering/Camera.h"
#include "AIngine/Constants.h"
#include "AIngine/Macros.h"
#include "Rendering/Viewport.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

AIngine::Rendering::Camera* AIngine::Rendering::Camera::s_instance = nullptr;

AIngine::Rendering::Camera::Camera(const Viewport & viewport, const glm::vec2& bounds, const glm::vec2& desiredScreenSizeInWorldUnits)
	: m_viewport(viewport)
{
	ASSERT(!s_instance, "There's already a Camera!");
	s_instance = this;

	glm::vec2 screenSize;

	if (desiredScreenSizeInWorldUnits == glm::vec2(0))
	{
		screenSize = bounds;
	}
	else {
		screenSize = desiredScreenSizeInWorldUnits;
	}

	m_zoom = viewport.GetViewportWidth() / screenSize.x;
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
	glm::vec2 worldToScreen = WorldToScreenPoint(pos);
	glm::vec2 offset = glm::vec2(m_viewport.GetViewportWidth() / 2.0, m_viewport.GetViewportHeight() / 2.0);
	m_position = worldToScreen - offset;
}

glm::mat4  AIngine::Rendering::Camera::GetViewMatrix() const
{
	return GetViewMatrix(glm::vec2(1.0));
}

glm::mat4 AIngine::Rendering::Camera::GetViewMatrix(const glm::vec2 & parallaxFactor) const
{
	return GetVirtualViewMatrix(parallaxFactor) * m_viewport.GetScaleMat();
}

glm::mat4 AIngine::Rendering::Camera::GetProjectionMatrix() const
{
	return glm::ortho(0.0f, (float)m_viewport.GetViewportWidth(), (float)m_viewport.GetViewportHeight(), 0.0f, -1.0f, 1.0f);
}

glm::vec2 AIngine::Rendering::Camera::ScreenToWorldPoint(const glm::vec2 & screenpoint) const
{
	glm::vec2 point = screenpoint - m_viewport.GetTopLeftCornerPosition();
	glm::mat4 inverseView = glm::inverse(GetViewMatrix());
	return inverseView * glm::vec4(point, 0, 1);
}

glm::vec2 AIngine::Rendering::Camera::WorldToScreenPoint(const glm::vec2 & worldpoint) const
{
	glm::vec2 point = worldpoint + ScreenToWorldPoint(m_viewport.GetTopLeftCornerPosition());
	return  GetViewMatrix() * glm::vec4(point, 0, 1);
}

glm::vec2 AIngine::Rendering::Camera::GetVisibleWorldSize() const
{

	glm::vec2 p1 = ScreenToWorldPoint(m_viewport.GetTopLeftCornerPosition());
	glm::vec2 p2 = ScreenToWorldPoint(m_viewport.GetTopLeftCornerPosition() + glm::vec2(m_viewport.GetViewportWidth(), m_viewport.GetViewportHeight()));

	return glm::vec2(abs(p2.x - p1.x), abs(p2.y - p1.y));
}

const AIngine::Rendering::Camera & AIngine::Rendering::Camera::Get()
{
	return *s_instance;
}


glm::mat4 AIngine::Rendering::Camera::GetVirtualViewMatrix(const glm::vec2 & parallaxFactor) const
{
	glm::mat4 mat = glm::mat4(1.0);
	mat = glm::translate(mat, glm::vec3(-m_position * parallaxFactor, 0.0));
	mat = glm::rotate(mat, m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	mat = glm::scale(mat, glm::vec3(m_zoom, m_zoom, 1.0));

	return mat;
}
