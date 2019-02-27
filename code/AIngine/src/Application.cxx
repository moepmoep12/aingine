#include "Application.h"
#include "Events/ApplicationEvents.h"
#include "log.h"
#include "Input.h"

#define BIND_EVENT_TO_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace AIngine {

	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		ASSERT(!s_instance, "Application already running");
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallbackFunction(BIND_EVENT_TO_FN(Application::OnEvent));
	}

	Application::~Application()
	{
		CORE_INFO("Core Application Destructor");
	}

	void Application::Run()
	{
		m_isRunning = true;

		glfwSetTime(0);

		while (m_isRunning)
		{
			glClearColor(0.5, 0.5, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : m_layerStack)
				layer->OnUpdate();

			m_window->OnUpdate();

			glfwSetTime(0);

			if (Input::IsMouseButtonPressed(0))
				DEBUG_INFO("Mouse button pressed");
		}

		// destroy our window
		m_window = NULL;

	}

	void Application::OnEvent(AIngine::Events::Event & e)
	{
		AIngine::Events::EventDispatcher dispatcher(e);

		// call the OnWindowClose function if its a windowclose event
		dispatcher.Dispatch<AIngine::Events::WindowCloseEvent>(BIND_EVENT_TO_FN(Application::OnWindowClose));


		// iterate through the layers to propagate the event
		for (auto it = m_layerStack.end(); it != m_layerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}

	}

	void Application::PushLayer(Layer * layer)
	{
		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer * overlay)
	{
		m_layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	float Application::GetDeltaTime()
	{
		return (float)glfwGetTime();
	}

	bool Application::OnWindowClose(AIngine::Events::WindowCloseEvent & e)
	{
		CORE_INFO("OnWindowClose");

		//stop running
		m_isRunning = false;

		return true;
	}
}