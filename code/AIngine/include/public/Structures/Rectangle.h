#pragma once

#include <glm/glm.hpp>

namespace AIngine::Structures {
	template<typename T>
	struct Rectangle {

		Rectangle()
			: x(0), y(0), width(0), height(0) {}

		Rectangle(T x, T y, T width, T height)
			: x(x), y(y), width(width), height(height) {}

		bool Contains(const glm::vec2& point) const {
			return (point.x >= x && point.x <= x + width)
				&& (point.y >= y && point.y <= y + height);
		}

		bool Contains(const Rectangle& other) const {
			return Contains(other.GetPosition())
				|| Contains(other.GetMax())
				|| Contains(other.GetBottomLeft())
				|| Contains(other.GetTopRight());
		}

		// corresponds to the top left corner
		glm::vec2 GetPosition() const {
			return glm::vec2(x, y);
		}

		// Set position of the top left corner
		void SetPosition(const glm::vec2& pos) {
			x = (T)pos.x;
			y = (T)pos.y;
		}

		glm::vec2 GetTopRight() const {
			return glm::vec2(x + width, y);
		}

		glm::vec2 GetBottomLeft() const {
			return glm::vec2(x, y + height);
		}

		glm::vec2 GetMax() const {
			return glm::vec2(x + width, y + height);
		}

		glm::vec2 GetCenter() const {
			return GetPosition() + glm::vec2(width / 2.0f, height / 2.0f);
		}

		T Area() const {
			return width * height;
		}

		enum Corner {
			TopLeft = 0,
			TopRight = 1,
			BottomRight = 2,
			BottomLeft = 3
		};

		T x, y, width, height;
	};

	typedef Rectangle<float> RectangleF;
	typedef Rectangle<int> RectangleI;
}