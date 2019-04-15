#include "Application.h"
#include "Events/ApplicationEvents.h"
#include "log.h"
#include "Input.h"
#include <memory>

#define BIND_EVENT_TO_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace AIngine {

	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		CORE_INFO("Starting Application...");

		ASSERT(!s_instance, "Application already running");
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallbackFunction(BIND_EVENT_TO_FN(Application::OnEvent));

		m_imGuiLayer = new AIngine::UI::ImGuiLayer();
		PushOverlay(m_imGuiLayer);

		{
			using namespace AIngine::Assets;

			m_assetRegistry.RegisterFactory<ShaderAsset>(
				std::move(
					std::unique_ptr<ShaderAssetFactory>(
						new ShaderAssetFactory()
					)
				)
				);

			m_assetRegistry.RegisterFactory<BitmapAsset>(
				std::move(std::unique_ptr<BitmapAssetFactory>(
					new BitmapAssetFactory()
					))
				);
		}
	}

	Application::~Application()
	{
		CORE_INFO("Destructor Application");
	}

	void Application::Run()
	{
		OnAppStartUp();

		CORE_INFO("App is running!");

		m_isRunning = true;

		glfwSetTime(0);

		while (m_isRunning)
		{
			glClearColor(0.5, 0.5, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : m_layerStack)
				layer->OnUpdate(GetDeltaTime());

			m_imGuiLayer->OnBegin();
			for (Layer* layer : m_layerStack)
				layer->OnImGuiRender();
			m_imGuiLayer->OnEnd();

			m_window->OnUpdate();

			//glfwSetTime(0);
		}

		CORE_INFO("Shutting App down...");

		OnAppShutDown();

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
		//return (1.0f / 60.0f);
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