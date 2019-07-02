#pragma once
#include "Structures/Rectangle.h"

#include <glm/glm.hpp>

namespace AIngine {
	class Window;
	class Application;
}

namespace AIngine::Rendering
{
	class Viewport final {
	public:
		inline int GetViewportWidth() const { return Rectangle.width; }
		inline int GetViewportHeight()const { return Rectangle.height; }
		inline float AspectRatio() const { return (float)Rectangle.width / (float)Rectangle.height; }
		inline glm::vec2 GetCenter() const { return Rectangle.GetCenter(); }
		inline glm::vec2 GetTopLeftCornerPosition() const { return Rectangle.GetPosition(); }
		inline const AIngine::Structures::RectangleI& GetRectangle() const { return Rectangle; }

		bool Contains(const glm::vec2& screenpoint) const;
		unsigned int GetWindowWidth() const;
		unsigned int GetWindowHeight() const;
		glm::mat4 GetScaleMat() const;
		glm::vec2 PointToScreen(const glm::vec2 viewpoint) const;

		// Creates a viewport with the specified width and height
		// width & height given in pixel
		// topleftX & topleftY refer to the screen coordinates of the topleft corner of the viewport
		Viewport(int viewportwidth, int viewportheight, int topleftX, int topleftY, const AIngine::Window& window);

		Viewport() = delete;
		Viewport(const Viewport& other) = delete;
		Viewport(Viewport&& other) = delete;
		Viewport operator=(const Viewport& other) = delete;
		Viewport operator=(Viewport&& other) = delete;

	private:
		friend class AIngine::Application;
		const Window& m_window;
		AIngine::Structures::RectangleI Rectangle;

	private:
		void Set(const glm::vec2& pos, unsigned int width, unsigned int height, bool preserveAspect = true);
	};
}