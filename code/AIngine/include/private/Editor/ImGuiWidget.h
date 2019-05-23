#pragma once

#include "imgui.h"
#include "Events/Event.h"

#include <glm/glm.hpp>

namespace AIngine::Editor {

	/*
	* The base class for widgets that make up the Editor
	*/
	class ImGuiWidget {
	public:

		/* Called once per frame when the widget will be drawn*/
		virtual void OnImGuiRender() = 0;

		/* can be used to react to input events*/
		virtual void OnEvent(AIngine::Events::Event& e) {}

		virtual const glm::vec2& GetSize() const { return m_size; }
		virtual const glm::vec2& GetPosition() const { return m_position; }

		virtual ~ImGuiWidget() {}

	protected:
		glm::vec2 m_size = glm::vec2(0);
		glm::vec2 m_position = glm::vec2(0);
	};
}