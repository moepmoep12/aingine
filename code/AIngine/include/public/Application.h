#pragma once

#include <memory>
#include "Window.h"
#include "Events/Event.h"
#include "Events/ApplicationEvents.h"

namespace AIngine {

	class Application {

	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(AIngine::Events::Event& e);
		float GetDeltaTime();

	private:
		bool OnWindowClose(AIngine::Events::WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_window;
		bool m_isRunning = false;

	};

	// to be defined by the actual Application
	Application* CreateApplication();
}