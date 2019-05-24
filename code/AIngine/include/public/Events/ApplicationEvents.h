#pragma once

#include "Events/Event.h"
#include "Structures/Rectangle.h"
#include <sstream>


namespace AIngine::Events {

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};



	class  WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
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

	class  WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(unsigned int xpos, unsigned int ypos)
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

	class WindowFocusEvent : public Event {
	public:
		WindowFocusEvent() {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Window Focus Event";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowFocus)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowFocusLostEvent : public Event {
	public:
		WindowFocusLostEvent() {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Window Focus Lost Event";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowLostFocus)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class  ViewportChangedEvent : public Event
	{
	public:
		ViewportChangedEvent(AIngine::Structures::Rectangle& viewportRect)
			: ViewportRect(viewportRect) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "ViewportChangedEvent: " << ViewportRect.x << ", " << ViewportRect.y << "  Size (" << ViewportRect.width << " | " << ViewportRect.height << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(ViewportChanged)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)

			AIngine::Structures::Rectangle& ViewportRect;
	};


	/*class  AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class  AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class  AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};*/
}