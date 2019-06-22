#pragma once
#include <glm/glm.hpp>

namespace AIngine {
	class Window;
	class Application;
}

namespace AIngine::Rendering
{
	class Viewport final {
	public:

		inline int GetViewportWidth() const { return m_width; }
		inline int GetViewportHeight()const { return m_height; }
		inline float AspectRatio() const { return (float)m_width / (float)m_height; }
		inline glm::vec2 GetCenter() const { return glm::vec2(m_x + m_width * 0.5, m_y + m_height * 0.5); }
		inline glm::vec2 GetTopLeftCornerPosition() const { return glm::vec2(m_x, m_y); }

		bool Contains(const glm::vec2& screenpoint) const;
		unsigned int GetWindowWidth() const;
		unsigned int GetWindowHeight() const;
		glm::mat4 GetScaleMat() const;
		glm::vec2 PointToScreen(const glm::vec2 viewpoint) const;

		// Creates a viewport with the specified width and height
		// width & height given in pixel
		// topleftX & topleftY refer to the screen coordinates of the topleft corner of the viewport
		Viewport(int viewportwidth, int viewportheight, int topleftX, int topleftY, const AIngine::Window& window);

	private:
		friend class AIngine::Application;
		unsigned int m_width, m_height, m_x, m_y;
		const Window& m_window;

	private:
		void Set(const glm::vec2& pos, unsigned int width, unsigned int height, bool preserveAspect = true);
	};
}