#include "UI/UIELement.h"
#include "AIngine/Input.h"
#include "AIngine/Macros.h"
#include "Rendering/UIRenderer.h"

namespace AIngine::UI {

	void UIElement::OnEvent(AIngine::Events::EventData & e)
	{
		AIngine::Events::EventDispatcher dispatcher(e);

		dispatcher.Dispatch<AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData>(BIND_EVENT_TO_FN(UIElement::OnMouseButtonPressed));
		dispatcher.Dispatch<AIngine::Events::MouseButtonReleasedEvent::MouseButtonReleasedEventData>(BIND_EVENT_TO_FN(UIElement::OnMouseButtonReleased));
	}

	bool AIngine::UI::UIElement::IsHovered() const
	{
		auto pair = AIngine::Input::GetMousePosition();
		return m_rectangle.Contains(glm::vec2(pair.first, pair.second));
	}

	bool UIElement::OnMouseButtonPressed(AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData & e)
	{
		if (e.GetMouseButton() == 0) {
			if (IsHovered()) {
				OnClicked();
				m_isClicked = true;
				return true;
			}
		}

		return false;
	}
	bool UIElement::OnMouseButtonReleased(AIngine::Events::MouseButtonReleasedEvent::MouseButtonReleasedEventData & e)
	{
		if (e.GetMouseButton() == 0) {
			if(m_isClicked) {
				m_isClicked = false;
				OnMouseReleased();
				return true;
			}
		}
		return false;
	}
	Canvas::~Canvas()
	{
		if (AIngine::Rendering::UIRenderer::canvas == GetOwner())
			AIngine::Rendering::UIRenderer::canvas = nullptr;
	}
}