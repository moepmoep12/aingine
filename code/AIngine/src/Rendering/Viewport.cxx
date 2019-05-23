#include "Rendering/Viewport.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Window.h"


bool AIngine::Rendering::Viewport::Contains(const glm::vec2 & screenpoint) const
{
	bool withinX = screenpoint.x >= m_x && screenpoint.x <= m_x + m_width;
	bool withinY = screenpoint.y >= m_y && screenpoint.y <= m_y + m_height;
	return withinX && withinY;
}

unsigned int AIngine::Rendering::Viewport::GetWindowWidth() const
{
	return m_window.GetWidth();
}

unsigned int AIngine::Rendering::Viewport::GetWindowHeight() const
{
	return m_window.GetHeight();
}

glm::mat4 AIngine::Rendering::Viewport::GetScaleMat() const
{
	glm::mat4 scalemat = glm::mat4(1.0);
	glm::vec2 scale = glm::vec2((float)m_width / (float)m_window.GetWidth(), (float)m_height / (float)m_window.GetHeight());
	scalemat = glm::scale(scalemat, glm::vec3(scale, 1.0));
	return scalemat;
}

glm::vec2 AIngine::Rendering::Viewport::PointToScreen(const glm::vec2 viewpoint) const
{
	glm::vec4 point(viewpoint, 0, 0);
	//point.x -= m_x;
	//point.y -= m_y;
	glm::mat4 scalemat = glm::inverse(GetScaleMat());
	return point * scalemat;
}

AIngine::Rendering::Viewport::Viewport(int viewportwidth, int viewportheight, int topleftX, int topleftY, const AIngine::Window& window)
	: m_width(viewportwidth), m_height(viewportheight), m_x(topleftX), m_y(topleftY), m_window(window)
{

}

void AIngine::Rendering::Viewport::Set(const glm::vec2 & pos, unsigned int width, unsigned int height, bool preserveAspect)
{
	if (preserveAspect) {
		float aspectRatio = m_window.GetWidth() / m_window.GetHeight();
		m_x = pos.x;
		m_y = pos.y;

		m_height = std::floorf(((float)width * ((float)m_window.GetHeight() / (float)m_window.GetWidth())));
		m_width = width;
	}
}
