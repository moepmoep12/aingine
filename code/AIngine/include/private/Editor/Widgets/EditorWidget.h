#pragma once

#include "imgui.h"
#include "Events/Event.h"
#include "Structures/Rectangle.h"

#include <glm/glm.hpp>

namespace AIngine::Editor {

	/*
	* The base class for widgets that make up the Editor
	*/
	class EditorWidget {
	public:

		/* Called once per frame when the widget will be drawn*/
		virtual void OnImGuiRender() = 0;

		/* can be used to react to input events*/
		virtual void OnEvent(AIngine::Events::EventData& e) {}

		virtual const glm::vec2& GetSize() const { return m_size; }
		virtual const glm::vec2& GetPosition() const { return m_position; }
		virtual bool IsWindowDocked() const { return m_isDocked; }
		virtual bool WasWindowSizeChanged() const { return m_wasSizeChanged; }

		virtual AIngine::Structures::Rectangle GetRectangle() const {
			return AIngine::Structures::Rectangle(m_position.x, m_position.y, m_size.x, m_size.y);
		}

		virtual ~EditorWidget() {}

	protected:
		glm::vec2 m_size = glm::vec2(0);
		glm::vec2 m_position = glm::vec2(0);
		bool m_isDocked = false;
		bool m_wasSizeChanged = true;
	};
}