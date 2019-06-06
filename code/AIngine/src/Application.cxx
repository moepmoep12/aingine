#include "Application.h"
#include "Events/ApplicationEvents.h"
#include "AIngine/Macros.h"
#include "AIngine/Input.h"
#include "Editor/Editor.h"
#include "Rendering/Renderer.h"
#include "Structures/SceneGraph.h"
#include "AIngine/Sounds.h"

#include <memory>

namespace AIngine {

	Application* Application::s_instance = nullptr;



	Application::Application()
	{
		CORE_INFO("Starting Application...");

		ASSERT(!s_instance, "Application already running");
		s_instance = this;

		// create window
		m_window = std::unique_ptr<Window>(Window::Create(m_windowConfig));
		m_window->SetEventCallbackFunction(BIND_EVENT_TO_FN(Application::OnEvent));

		// Create UI Layer
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

			m_assetRegistry.RegisterFactory<SoundAsset>(
				std::move(std::unique_ptr<SoundAssetFactory>(
					new SoundAssetFactory()
					))
				);
		}

		// load shader
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

		//load basic white texture
		path = std::string("assets/Intellgine/textures/White.png");
		AIngine::Assets::BitmapAsset* bitmap = m_assetRegistry.Load<AIngine::Assets::BitmapAsset>(path);

		m_Graphics = new AIngine::Graphics();

		PushLayer(new Sounds());
	}

	Application::~Application()
	{
		CORE_INFO("Destructor Application");
	}

	static float s_currentFrame = 0.0f;
	static float s_lastFrame = 0.0f;

	void Application::Run()
	{
		// create game world
		m_world = new World(m_bounds, m_gravity);
		PushLayer(m_world);

		// create viewport
		m_viewport = new AIngine::Rendering::Viewport(m_window->GetWidth(), m_window->GetHeight(), 0, 0, *m_window.get());

		// create camera
		m_camera = new AIngine::Rendering::Camera(*m_viewport, glm::vec2(m_bounds.y - m_bounds.x, m_bounds.z - m_bounds.w));


#ifdef _DEBUG
		m_editor = new AIngine::Editor::Editor();
		PushOverlay(m_editor);
#endif

		m_renderer->initRenderData();

		OnAppStartUp();

		CORE_INFO("App is running!");

		m_isRunning = true;

		glfwSetTime(0);

		// the game loop
		while (m_isRunning)
		{
			// calc delta time
			s_currentFrame = (float)glfwGetTime();
			m_deltaTime = s_currentFrame - s_lastFrame;
			s_lastFrame = s_currentFrame;

			// handle user input
			m_window->PollInput();

			OnAppUpdate();

			// update logic
			for (AIngine::Structures::Layer* layer : m_layerStack)
				layer->OnUpdate(GetDeltaTime());

			// scene rendering
			m_renderer->Traverse(&m_world->GetSceneGraph().GetRoot());

			// ui rendering
			m_imGuiLayer->OnBegin();
			for (AIngine::Structures::Layer* layer : m_layerStack) {
				layer->OnImGuiRender();
			}
			// render debug stuff last
			m_Graphics->Flush();
			// finish UI rendering
			m_imGuiLayer->OnEnd();

			// finish the frame
			m_window->OnUpdate();
		}

		CORE_INFO("Shutting App down...");

		OnAppShutDown();

		m_window = NULL;
		delete m_renderer;
		delete m_camera;
		delete m_Graphics;
		delete m_viewport;


#ifdef _DEBUG
		m_layerStack.PopOverlay(m_editor);
		delete m_editor;
#endif
	}

	void Application::OnEvent(AIngine::Events::Event & e)
	{
		AIngine::Events::EventDispatcher dispatcher(e);

		// call the OnWindowClose function if its a windowclose event
		dispatcher.Dispatch<AIngine::Events::WindowCloseEvent>(BIND_EVENT_TO_FN(Application::OnWindowClose));
		dispatcher.Dispatch<AIngine::Events::WindowResizeEvent>(BIND_EVENT_TO_FN(Application::OnWindowResize));

		// propagate it to the game
		OnAppEvent(e);

		// iterate through the layers to propagate the event
		for (auto it = m_layerStack.end(); it != m_layerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}

		dispatcher.Dispatch<AIngine::Events::ViewportChangedEvent>(BIND_EVENT_TO_FN(Application::OnViewportChanged));
	}

	void Application::PushLayer(AIngine::Structures::Layer * layer)
	{
		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(AIngine::Structures::Layer * overlay)
	{
		m_layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	float Application::GetDeltaTime()
	{
		return m_deltaTime;
	}

	bool Application::IsAnyUiElementHovered() const
	{
		if (m_editor)
			return m_editor->IsAnyUIElementHovered();
		else
			return false;
	}

	/* Event handling */

	bool Application::OnWindowClose(AIngine::Events::WindowCloseEvent & e)
	{
		CORE_INFO(e.ToString());

		//stop running
		m_isRunning = false;

		return true;
	}

	bool Application::OnWindowResize(AIngine::Events::WindowResizeEvent & e)
	{
		m_renderer->SetViewport();
		CORE_INFO(e.ToString());
		return true;
	}

	bool Application::OnViewportChanged(AIngine::Events::ViewportChangedEvent & e)
	{
		m_viewport->Set(e.ViewportRect.GetPosition(), e.ViewportRect.width, e.ViewportRect.height);
		m_camera->SetZoom((float)m_window->GetWidth() / (float)m_world->GetBounds().y);
		m_renderer->SetViewport();
		CORE_INFO("Viewport Size Changed To ({0} | {1}) at Position ({2} | {3})", m_viewport->m_width, m_viewport->m_height, m_viewport->m_x, m_viewport->m_y);
		return true;
	}
}