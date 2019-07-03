#include "UI/UIELement.h"
#include "AIngine/Input.h"
#include "AIngine/Macros.h"
#include "Rendering/UIRenderer.h"
#include "Rendering/Viewport.h"
#include "Application.h"

namespace AIngine::UI {

	AIngine::Structures::Rectangle<int> UIElement::GetRectangle() const
	{
		glm::vec2 offset = GetOffsetFromAnchor(AnchorPos);
		const AIngine::Structures::RectangleI& viewportRect = AIngine::Application::GetViewport().GetRectangle();

		return AIngine::Structures::RectangleI(offset.x + (m_rectangle.x / TargetResolution.x) * viewportRect.width, offset.y + (m_rectangle.y / TargetResolution.y) * viewportRect.height,
			(m_rectangle.width / TargetResolution.x) * viewportRect.width, (m_rectangle.height / TargetResolution.y) * viewportRect.height);
	}

	void UIElement::SetAnchor(Anchor anch)
	{
		AnchorPos = anch;
	}

	void UIElement::OnEvent(AIngine::Events::EventData & e)
	{
		AIngine::Events::EventDispatcher dispatcher(e);

		dispatcher.Dispatch<AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData>(BIND_EVENT_TO_FN(UIElement::OnMouseButtonPressed));
		dispatcher.Dispatch<AIngine::Events::MouseButtonReleasedEvent::MouseButtonReleasedEventData>(BIND_EVENT_TO_FN(UIElement::OnMouseButtonReleased));
	}

	bool AIngine::UI::UIElement::IsHovered() const
	{
		auto pair = AIngine::Input::GetMousePosition();
		return GetRectangle().Contains(glm::vec2(pair.first, pair.second));
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
			if (m_isClicked) {
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

	glm::vec2 GetOffsetFromAnchor(Anchor pos)
	{
		glm::vec2 offset(0);
		const AIngine::Structures::RectangleI& viewportRect = AIngine::Application::GetViewport().GetRectangle();

		switch (pos) {
		case AIngine::UI::Anchor::TopLeft:
			offset = glm::vec2(0);
			break;

		case AIngine::UI::Anchor::TopRight:
			offset = viewportRect.GetTopRight();
			break;

		case AIngine::UI::Anchor::BottomRight:
			offset = viewportRect.GetMax();
			break;

		case AIngine::UI::Anchor::BottomLeft:
			offset = viewportRect.GetBottomLeft();
			break;

		case AIngine::UI::Anchor::Center:
			offset = viewportRect.GetCenter();
			break;

		case AIngine::UI::Anchor::CenterDown:
			glm::highp_ivec2 centerDown = viewportRect.GetMax();
			centerDown.x -= viewportRect.width * 0.5f;
			offset = centerDown;
			break;

		case AIngine::UI::Anchor::CenterLeft:
			glm::highp_ivec2 centerLeft = viewportRect.GetPosition();
			centerLeft.y += viewportRect.height * 0.5f;
			offset = centerLeft;
			break;

		case AIngine::UI::Anchor::CenterRight:
			glm::highp_ivec2 centerRight = viewportRect.GetTopRight();
			centerRight.y += viewportRect.height * 0.5f;
			offset = centerRight;
			break;

		case AIngine::UI::Anchor::CenterUp:
			glm::highp_ivec2 centerUp = viewportRect.GetTopRight();
			centerUp.x -= viewportRect.width * 0.5f;
			offset = centerUp;
			break;
		}
		return offset;
	}
}