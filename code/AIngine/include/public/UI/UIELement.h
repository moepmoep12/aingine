#pragma once
#include "Structures/Rectangle.h"
#include "AIngine/Component.h"
#include "Events/InputEvents.h"

namespace AIngine::Rendering {
	class GLShaderProgram;
}

namespace AIngine::UI {

	enum class Anchor {
		Center = 0,
		TopLeft,
		TopRight,
		BottomRight,
		BottomLeft,
		CenterUp,
		CenterLeft,
		CenterDown,
		CenterRight
	};

	const glm::vec2 TargetResolution(1920, 1080);

	glm::vec2 GetOffsetFromAnchor(Anchor pos);

	class UIElement : public AIngine::Component {
	public:
		AIngine::Structures::Rectangle<int> GetRectangle() const;
		inline const AIngine::Structures::Rectangle<int>& GetRectangleNative() const { return m_rectangle; }
		inline void SetRectangle(const AIngine::Structures::Rectangle<int>& rect) { m_rectangle = rect; }

		inline bool IsDisabled() const { return m_isDisabled; }
		inline void SetDisabled(bool disabled) { m_isDisabled = disabled; }

		inline bool IsClicked() const { return m_isClicked; }

		inline Anchor GetAnchor() const { return AnchorPos; }
		void SetAnchor(Anchor anch);

		/* Sets the position of the top left corner */
		inline virtual void SetPosition(const glm::vec2& newpos) {
			m_rectangle.x = newpos.x;
			m_rectangle.y = newpos.y;
		}

		inline virtual void SetWidth(int width) { m_rectangle.width = width; }
		inline virtual void SetHeight(int height) { m_rectangle.height = height; }

		inline virtual bool Render(AIngine::Rendering::GLShaderProgram& shader) const = 0;
		virtual void Update(float deltatime) {}

		// Inherited via Component
		virtual void OnEvent(AIngine::Events::EventData& e);

		bool IsHovered() const;

		glm::vec4 TintColor = glm::vec4(1);
		glm::vec4 DisabledColor = glm::vec4(0.2, 0.2, 0.2, 1);

	protected:
		virtual void OnClicked() {}
		virtual void OnMouseReleased() {}

	protected:
		bool m_isClicked = false;
		bool m_isDisabled = false;
		Anchor AnchorPos = Anchor::TopLeft;

	private:
		bool OnMouseButtonPressed(AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData& e);
		bool OnMouseButtonReleased(AIngine::Events::MouseButtonReleasedEvent::MouseButtonReleasedEventData& e);

		virtual void OnUpdate(float deltatime) override;

	private:
		AIngine::Structures::Rectangle<int> m_rectangle = AIngine::Structures::Rectangle<int>(0, 0, 5, 2);

	};

	class Canvas : public UIElement {
	public:
		virtual ~Canvas();
		inline virtual bool Render(AIngine::Rendering::GLShaderProgram& shader) const override { return false; }
	};
}