#pragma once
#include "Structures/Rectangle.h"
#include "AIngine/Component.h"
#include "Events/InputEvents.h"

namespace AIngine::UI {

	class UIElement : public AIngine::Component {
	public:

		inline const AIngine::Structures::Rectangle<int>& GetRectangle() const { return m_rectangle; }

		inline bool IsDisabled() const { return m_isDisabled; }
		inline void SetDisabled(bool disabled) { m_isDisabled = disabled; }

		inline virtual void Render(const glm::mat4& modelMatrix) const = 0;

		// Inherited via Component
		virtual void OnEvent(AIngine::Events::EventData& e);

		bool IsHovered();

		glm::vec2 ScreenPosition = glm::vec2(0);

		glm::vec4 TintColor = glm::vec4(1);
		glm::vec4 DisabledColor = glm::vec4(0.2, 0.2, 0.2, 1);

	protected:
		virtual void OnClicked() {}
		virtual void OnMouseReleased() {}

	protected:
		AIngine::Structures::Rectangle<int> m_rectangle;
		bool m_isClicked = false;
		bool m_isDisabled = false;

	private:
		bool OnMouseButtonPressed(AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData& e);
		bool OnMouseButtonReleased(AIngine::Events::MouseButtonReleasedEvent::MouseButtonReleasedEventData& e);
	};
}