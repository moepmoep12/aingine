#include "Application.h"
#include "Events/ApplicationEvents.h"
#include "log.h"

namespace AIngine {

	Application::Application()
	{
		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallbackFunction(std::bind(&Application::OnEvent, this, std::placeholders::_1));
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
			for (Layer* layer : m_layerStack)
				layer->OnUpdate();

			m_window->OnUpdate();

			glfwSetTime(0);
		}

		// destroy our window
		m_window = NULL;

	}

	void Application::OnEvent(AIngine::Events::Event & e)
	{
		AIngine::Events::EventDispatcher dispatcher(e);

		// call the OnWindowClose function if its a windowclose event
		dispatcher.Dispatch<AIngine::Events::WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));


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
	}

	void Application::PushOverlay(Layer * overlay)
	{
		m_layerStack.PushOverlay(overlay);
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