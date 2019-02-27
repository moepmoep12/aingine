#pragma once

#include <memory>
#include "Window.h"
#include "Events/Event.h"
#include "Events/ApplicationEvents.h"
//#include "Layer.h"
#include "LayerStack.h"

namespace AIngine {

	class Application {

	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(AIngine::Events::Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		float GetDeltaTime();

	private:
		bool OnWindowClose(AIngine::Events::WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_window;
		bool m_isRunning = false;
		LayerStack m_layerStack;

	};

	// to be defined by the actual Application
	Application* CreateApplication();
}