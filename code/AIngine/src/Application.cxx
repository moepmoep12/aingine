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

		// create window
		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallbackFunction(BIND_EVENT_TO_FN(Application::OnEvent));

		m_imGuiLayer = new AIngine::UI::ImGuiLayer();
		PushOverlay(m_imGuiLayer);

		// create asset factories
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

		// create scenegraph
		m_sceneGraph = new SceneGraph();
		PushOverlay(m_sceneGraph);

		std::string vertexPath("assets/Intellgine/shader/screenshader/vertexScreen.glsl");
		std::string fragPath("assets/Intellgine/shader/screenshader/fragmentScreen.glsl");
		std::string path;
		path.append(vertexPath);
		path.append(";");
		path.append(fragPath);

		AIngine::Assets::ShaderAsset* shaderAsset = m_assetRegistry.Load<AIngine::Assets::ShaderAsset>(path);

		CORE_INFO("Loaded ShaderProgram with {0} ", shaderAsset->GetShader().GetID());

		// create sprite renderer
		m_renderer = new AIngine::Rendering::SpriteRenderer(&shaderAsset->GetShader());

	}

	Application::~Application()
	{
		CORE_INFO("Destructor Application");
	}

	static float s_currentFrame = 0.0f;
	static float s_lastFrame = 0.0f;

	void Application::Run()
	{
		OnAppStartUp();

		CORE_INFO("App is running!");

		m_isRunning = true;

		glfwSetTime(0);

		// the game loop
		while (m_isRunning)
		{
			// calc delta time
			s_currentFrame = glfwGetTime();
			m_deltaTime = s_currentFrame - s_lastFrame;
			s_lastFrame = s_currentFrame;

			// handle user input
			m_window->PollInput();

			OnAppUpdate();

			// update logic
			for (Layer* layer : m_layerStack)
				layer->OnUpdate(GetDeltaTime());

			// scene rendering
			m_renderer->Render(&m_sceneGraph->GetRoot());

			// ui rendering
			m_imGuiLayer->OnBegin();
			for (Layer* layer : m_layerStack)
				layer->OnImGuiRender();
			m_imGuiLayer->OnEnd();

			// finish the frame
			m_window->OnUpdate();
		}

		CORE_INFO("Shutting App down...");

		OnAppShutDown();

		// destroy our window
		m_window = NULL;
		delete m_renderer;
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
		return m_deltaTime;
	}

	bool Application::OnWindowClose(AIngine::Events::WindowCloseEvent & e)
	{
		CORE_INFO(e.ToString());

		//stop running
		m_isRunning = false;

		return true;
	}
}