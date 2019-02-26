#include "Application.h"
#include "ApplicationEvents.h"
#include "log.h"

namespace AIngine {

	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		AIngine::Events::WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(AIngine::Events::EventCategoryApplication))
		{
			DEBUG_TRACE(e);
		}
		if (e.IsInCategory(AIngine::Events::EventCategoryInput))
		{
			DEBUG_TRACE(e);
		}
	}
}