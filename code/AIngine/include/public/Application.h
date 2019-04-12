#pragma once

#include <memory>
#include "Window.h"
#include "Events/Event.h"
#include "Events/ApplicationEvents.h"
#include "UI/ImGuiLayer.h"
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

		inline static Application& Get() { return *s_instance; }
		inline const Window& GetWindow() const { return *m_window; }

	private:
		bool OnWindowClose(AIngine::Events::WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_window;
		bool m_isRunning = false;
		LayerStack m_layerStack;
		AIngine::UI::ImGuiLayer* m_imGuiLayer;
		static Application* s_instance;

	};

	// to be defined by the actual Application
	Application* CreateApplication();
}