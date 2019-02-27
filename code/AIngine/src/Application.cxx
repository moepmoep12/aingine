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
			m_window->OnUpdate();

			//DEBUG_INFO(GetDeltaTime());

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

		//CORE_TRACE("{0}", e);
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