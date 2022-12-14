#pragma once

#include "Events/Event.h"
#include "AIngine/KeyCodes.h"
#include <sstream>

namespace AIngine::Events {

	class  MouseMovedEvent : public Event<void, float, float>
	{
	public:
		class MouseMovedEventData : public EventData {
		public:
			MouseMovedEventData(float x, float y)
				: m_MouseX(x), m_MouseY(y) {}

			inline float GetX() const { return m_MouseX; }
			inline float GetY() const { return m_MouseY; }

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
				return ss.str();
			}

			EVENT_CLASS_TYPE(MouseMoved)
				EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)


		private:
			float m_MouseX, m_MouseY;
		};

	};



	class  MouseScrolledEvent : public Event<void, float, float>
	{
	public:
		class MouseScrolledEventData : public EventData {
		public:
			MouseScrolledEventData(float xOffset, float yOffset)
				: m_XOffset(xOffset), m_YOffset(yOffset) {}

			inline float GetXOffset() const { return m_XOffset; }
			inline float GetYOffset() const { return m_YOffset; }

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
				return ss.str();
			}

			EVENT_CLASS_TYPE(MouseScrolled)
				EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)


		private:
			float m_XOffset, m_YOffset;
		};

	};



	class  MouseButtonEvent : public Event<void, MouseButton>
	{
	public:
		class MouseButtonEventData : public EventData {
		public:
			inline MouseButton GetMouseButton() const { return m_Button; }

			EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)


		protected:
			MouseButtonEventData(MouseButton button)
				: m_Button(button) {}

			MouseButton m_Button;
		};
	};



	class  MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		class MouseButtonPressedEventData : public MouseButtonEventData {
		public:
			MouseButtonPressedEventData(MouseButton button)
				: MouseButtonEventData(button) {}

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "MouseButtonPressedEvent: " << static_cast<int>(m_Button);
				return ss.str();
			}

			EVENT_CLASS_TYPE(MouseButtonPressed)
		};
	};



	class  MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		class MouseButtonReleasedEventData : public MouseButtonEventData {
		public:
			MouseButtonReleasedEventData(MouseButton button)
				: MouseButtonEventData(button) {}

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "MouseButtonReleasedEvent: " << static_cast<int>(m_Button);
				return ss.str();
			}

			EVENT_CLASS_TYPE(MouseButtonReleased)
		};
	};

	//**************************************************************************************
	//**************************************************************************************
	//  ----------------------- KEY EVENTS -------------------------------------------------
	//**************************************************************************************
	//**************************************************************************************

	class  KeyEvent : public Event<void, KeyCode>
	{
	public:
		class KeyEventData : public EventData {
		public:
			inline KeyCode GetKeyCode() const { return m_KeyCode; }

			EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

		protected:
			KeyEventData(AIngine::KeyCode keycode)
				: m_KeyCode(keycode) {}

			AIngine::KeyCode m_KeyCode;
		};
	};



	class  KeyPressedEvent : public KeyEvent
	{
	public:
		class KeyPressedEventData : public KeyEventData {
		public:
			KeyPressedEventData(AIngine::KeyCode keycode, int repeatCount)
				: KeyEventData(keycode), m_RepeatCount(repeatCount) {}

			inline int GetRepeatCount() const { return m_RepeatCount; }

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyPressedEvent: " << static_cast<int>(m_KeyCode) << " (" << m_RepeatCount << " repeats)";
				return ss.str();
			}

			EVENT_CLASS_TYPE(KeyPressed)

		private:
			int m_RepeatCount;
		};
	};



	class  KeyReleasedEvent : public KeyEvent
	{
	public:
		class KeyReleasedEventData : public KeyEventData {
		public:
			KeyReleasedEventData(AIngine::KeyCode keycode)
				: KeyEventData(keycode) {}

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyReleasedEvent: " << static_cast<int>(m_KeyCode);
				return ss.str();
			}

			EVENT_CLASS_TYPE(KeyReleased)
		};
	};

	class  KeyTypedEvent : public KeyEvent
	{
	public:
		class KeyTypedEventData : public KeyEventData {
		public:
			KeyTypedEventData(AIngine::KeyCode keycode)
				: KeyEventData(keycode) {}

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyTypedEvent: " << static_cast<int>(m_KeyCode);
				return ss.str();
			}

			EVENT_CLASS_TYPE(KeyTyped)
		};
	};
}