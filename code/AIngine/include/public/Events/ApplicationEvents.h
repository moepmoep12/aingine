#pragma once

#include "Events/Event.h"
#include "Structures/Rectangle.h"
#include <sstream>


namespace AIngine::Events {

	// WindowCloseEvent
	class WindowCloseEvent : public Event<void>
	{
	public:
		WindowCloseEvent() {}

		class WindowCloseEventData : public EventData {
		public:
			EVENT_CLASS_TYPE(WindowClose)
				EVENT_CLASS_CATEGORY(EventCategoryApplication)
		};
	};


	// WindowResizeEvent
	class  WindowResizeEvent : public Event<void, unsigned int, unsigned int>
	{
	public:
		class WindowResizeEventData : public EventData {
		public:
			WindowResizeEventData(unsigned int width, unsigned int height)
				: m_Width(width), m_Height(height) {}

			inline unsigned int GetWidth() const { return m_Width; }
			inline unsigned int GetHeight() const { return m_Height; }

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "Window Size: " << m_Width << ", " << m_Height;
				return ss.str();
			}

			EVENT_CLASS_TYPE(WindowResize)
				EVENT_CLASS_CATEGORY(EventCategoryApplication)

		private:
			unsigned int m_Width, m_Height;
		};
	};

	// WindowMovedEvent
	class  WindowMovedEvent : public Event<void, unsigned int, unsigned int>
	{
	public:
		class WindowMovedEventData : public EventData {
		public:
			WindowMovedEventData(unsigned int xpos, unsigned int ypos)
				: m_x(xpos), m_y(ypos) {}

			inline unsigned int GetX() const { return m_x; }
			inline unsigned int GetY() const { return m_y; }

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "Window Position: " << m_x << ", " << m_y;
				return ss.str();
			}

			EVENT_CLASS_TYPE(WindowMoved)
				EVENT_CLASS_CATEGORY(EventCategoryApplication)

		private:
			unsigned int m_x, m_y;
		};

	};

	// WindowFocusEvent
	class WindowFocusEvent : public Event<void> {
	public:
		WindowFocusEvent() {}
		class WindowFocusEventData : public EventData {
			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "Window Focus Event";
				return ss.str();
			}

			EVENT_CLASS_TYPE(WindowFocus)
				EVENT_CLASS_CATEGORY(EventCategoryApplication)
		};
	};

	// WindowFocusLostEvent
	class WindowFocusLostEvent : public Event<void> {
	public:
		WindowFocusLostEvent() {}
		class WindowsFocusLostEventData : public EventData {
			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "Window Focus Lost Event";
				return ss.str();
			}

			EVENT_CLASS_TYPE(WindowLostFocus)
				EVENT_CLASS_CATEGORY(EventCategoryApplication)
		};
	};

	// ViewportChangedEvent
	class  ViewportChangedEvent : public Event<void, AIngine::Structures::RectangleI&>
	{
	public:
		class ViewportChangedEventData : public EventData {
		public:
			ViewportChangedEventData(AIngine::Structures::RectangleI& viewportRect)
				: ViewportRect(viewportRect) {}

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "ViewportChangedEvent: " << ViewportRect.x << ", " << ViewportRect.y << "  Size (" << ViewportRect.width << " | " << ViewportRect.height << ")";
				return ss.str();
			}

			EVENT_CLASS_TYPE(ViewportChanged)
				EVENT_CLASS_CATEGORY(EventCategoryApplication)

				AIngine::Structures::RectangleI& ViewportRect;
		};
	};

	// EnterPlayMode
	class EnterPlayModeEventData : public EventData {
		
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Enter Playmode Event";
			return ss.str();
		}

		EVENT_CLASS_TYPE(EnterPlayMode)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	// ExitPlayMode
	class ExitPlayModeEventData : public EventData {

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Exit Playmode Event";
			return ss.str();
		}

		EVENT_CLASS_TYPE(ExitPlayMode)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};


}