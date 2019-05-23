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
			ss << "ViewportResize: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_Width, m_Height;
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