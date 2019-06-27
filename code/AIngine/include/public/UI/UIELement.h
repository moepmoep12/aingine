#pragma once
#include "Structures/Rectangle.h"
#include "AIngine/Component.h"
#include "Events/InputEvents.h"

namespace AIngine::Rendering {
	class GLShaderProgram;
}

namespace AIngine::Editor {
	class ButtonComponentWidget;
}

namespace AIngine::UI {

	class UIElement : public AIngine::Component {
	public:
		friend class AIngine::Editor::ButtonComponentWidget;

		inline const AIngine::Structures::Rectangle<int>& GetRectangle() const { return m_rectangle; }
		inline void SetRectangle(const AIngine::Structures::Rectangle<int>& rect) { m_rectangle = rect; }

		inline bool IsDisabled() const { return m_isDisabled; }
		inline void SetDisabled(bool disabled) { m_isDisabled = disabled; }

		inline virtual void Render(const glm::mat4& modelMatrix, AIngine::Rendering::GLShaderProgram& shader) const = 0;

		// Inherited via Component
		virtual void OnEvent(AIngine::Events::EventData& e);

		bool IsHovered() const;

		glm::vec4 TintColor = glm::vec4(1);
		glm::vec4 DisabledColor = glm::vec4(0.2, 0.2, 0.2, 1);

	protected:
		virtual void OnClicked() {}
		virtual void OnMouseReleased() {}

	protected:
		AIngine::Structures::Rectangle<int> m_rectangle = AIngine::Structures::Rectangle<int>(0, 0, 5, 2);
		bool m_isClicked = false;
		bool m_isDisabled = false;

	private:
		bool OnMouseButtonPressed(AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData& e);
		bool OnMouseButtonReleased(AIngine::Events::MouseButtonReleasedEvent::MouseButtonReleasedEventData& e);
	};

	class Canvas : public UIElement {
	public:
		virtual ~Canvas();
		inline virtual void Render(const glm::mat4& modelMatrix, AIngine::Rendering::GLShaderProgram& shader) const override {}
	};
}