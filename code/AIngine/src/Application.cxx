#include "Application.h"
#include "Events/ApplicationEvents.h"
#include "AIngine/Macros.h"
#include "AIngine/Input.h"
#include "Editor/Editor.h"
#include "Rendering/Renderer.h"
#include "Structures/SceneGraph.h"
#include "AIngine/Sounds.h"
#include "Rendering/Font.h"
#include "AIngine/Script.h"
#include "Rendering/ParticleRenderer.h"

#include <memory>

namespace AIngine {

	Application* Application::s_instance = nullptr;



	Application::Application()
	{
		CORE_INFO("Starting Application...");

		ASSERT(!s_instance, "Application already running");
		s_instance = this;

		// create window & register callbacks
		m_window = std::unique_ptr<Window>(Window::Create(m_windowConfig));
		RegisterCallbacks();

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

			m_assetRegistry.RegisterFactory<FontAsset>(
				std::move(std::unique_ptr<FontAssetFactory>(
					new FontAssetFactory()
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

		// create Graphics API
		m_Graphics = new AIngine::Graphics();

		//create Sound API
		PushLayer(new SoundEngine(m_window->GetWin32Window()));

		// create viewport
		m_viewport = new AIngine::Rendering::Viewport(m_window->GetWidth(), m_window->GetHeight(), 0, 0, *m_window.get());

		// create camera
		m_camera = new AIngine::Rendering::Camera(*m_viewport, glm::vec2(m_bounds.y - m_bounds.x, m_bounds.z - m_bounds.w));

		// create particle renderer
		m_particleRenderer = new AIngine::Rendering::ParticleRenderer();
	}

	Application::~Application()
	{
	}

	static float s_currentFrame = 0.0f;
	static float s_lastFrame = 0.0f;

	void Application::Run()
	{
		// create game world
		m_world = new World(m_bounds, m_gravity);
		PushLayer(m_world);

		// in order to compile the template we need to dummy-call it
		m_world->GetSceneGraph().GetRoot().AddComponent<Script>();
		m_world->GetSceneGraph().GetRoot().GetComponent<Script>();
		m_world->GetSceneGraph().GetRoot().RemoveComponent<Script>();

		// create Editor if we're in Debug
#ifdef _DEBUG
		m_editor = new AIngine::Editor::Editor();
		PushOverlay(m_editor);
		m_editor->OnViewportChangedEvent += [=](AIngine::Structures::RectangleI& viewport) {
			this->OnViewportChanged(viewport);
		};
		m_editor->OnEnterPlayModeEvent += [=]() {
			this->OnEnterPlayMode();
		};
		m_editor->OnLeavePlayModeEvent += [=]() {
			this->OnLeavePlayMode();
		};

#else
		AIngine::Structures::Rectangle viewportRect = AIngine::Structures::RectangleI(m_window->GetX(), 0, m_window->GetWidth(), m_window->GetHeight());
		OnViewportChanged(viewportRect);
		LoadScene(0);
		OnEnterPlayMode();
#endif

		m_renderer->initRenderData();

		if ((AIngine::Editor::Editor::GetIsInPlayMode())) {
			OnAppStartUp();
		}

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

			if ((AIngine::Editor::Editor::IsGameRunning()))
				// update game
				OnAppUpdate();

			// update logic
			for (AIngine::Structures::Layer* layer : m_layerStack)
				layer->OnUpdate(GetDeltaTime());

			// scene rendering
			m_renderer->Traverse(&m_world->GetSceneGraph().GetRoot());
			m_particleRenderer->Traverse(&m_world->GetSceneGraph().GetRoot());

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

#ifndef _DEBUG
		OnLeavePlayMode();
#endif
		// Clean up
		m_window = NULL;
		delete m_renderer;
		delete m_camera;
		delete m_Graphics;
		delete m_viewport;
		delete m_particleRenderer;

#ifdef _DEBUG
		m_layerStack.PopOverlay(m_editor);
		delete m_editor;
#endif
	}

	void Application::PropagateEventData(AIngine::Events::EventData & e)
	{
		for (auto it = m_layerStack.end(); it != m_layerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}

		if ((AIngine::Editor::Editor::IsGameRunning()))
			// propagate it to the game
			OnAppEvent(e);

	}

	void Application::OnEnterPlayMode()
	{
		PropagateEventData(AIngine::Events::EnterPlayModeEventData());
		OnAppStartUp();
		//AIngine::Structures::OnStartTraverser onStartTraverser;
		//onStartTraverser.Traverse(&m_world->GetSceneGraph().GetRoot());

	}

	void Application::OnLeavePlayMode()
	{
		PropagateEventData(AIngine::Events::ExitPlayModeEventData());
		OnAppShutDown();
		//AIngine::Structures::OnEndTraverser onEndTraverser;
		//onEndTraverser.Traverse(&m_world->GetSceneGraph().GetRoot());
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

	void Application::LoadScene(int index)
	{
		if (s_instance) {
			std::vector<AIngine::Editor::Scene> scenes = AIngine::Editor::Editor::LoadBuildScenes();
			if (index < scenes.size()) {
				AIngine::Editor::Editor::ResetSceneGraph(s_instance);
				AIngine::Editor::Editor::LoadScene(scenes[index].Name);
			}
		}
	}

	bool Application::IsRunning()
	{
		return AIngine::Editor::Editor::IsGameRunning();
	}

	void Application::RegisterCallbacks()
	{
		m_window->GetWindowData().OnKeyPressedEvent += [=](AIngine::KeyCodes key) {
			this->PropagateEventData(AIngine::Events::KeyPressedEvent::KeyPressedEventData(key, 1));
		};

		m_window->GetWindowData().OnKeyReleasedEvent += [=](AIngine::KeyCodes key) {
			this->PropagateEventData(AIngine::Events::KeyReleasedEvent::KeyReleasedEventData(key));
		};

		m_window->GetWindowData().OnKeyTypedEvent += [=](AIngine::KeyCodes key) {
			this->PropagateEventData(AIngine::Events::KeyTypedEvent::KeyTypedEventData(key));
		};

		m_window->GetWindowData().OnMouseButtonPressedEvent += [=](int button) {
			this->PropagateEventData(AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData(button));
		};

		m_window->GetWindowData().OnMouseButtonReleasedEvent += [=](int button) {
			this->PropagateEventData(AIngine::Events::MouseButtonReleasedEvent::MouseButtonReleasedEventData(button));
		};

		m_window->GetWindowData().OnMouseMovedEvent += [=](float xpos, float ypos) {
			this->PropagateEventData(AIngine::Events::MouseMovedEvent::MouseMovedEventData(xpos, ypos));
		};

		m_window->GetWindowData().OnMouseScrolledEvent += [=](float xoffset, float yoffset) {
			this->PropagateEventData(AIngine::Events::MouseScrolledEvent::MouseScrolledEventData(xoffset, yoffset));
		};

		m_window->GetWindowData().OnWindowClosedEvent += [=]() {
			this->OnWindowClose();
		};

		m_window->GetWindowData().OnWindowFocusEvent += [=]() {
			this->PropagateEventData(AIngine::Events::WindowFocusEvent::WindowFocusEventData());
		};

		m_window->GetWindowData().OnWindowFocusLostEvent += [=]() {
			this->PropagateEventData(AIngine::Events::WindowFocusLostEvent::WindowsFocusLostEventData());
		};

		m_window->GetWindowData().OnWindowMovedEvent += [=](unsigned int x, unsigned int y) {
			this->PropagateEventData(AIngine::Events::WindowMovedEvent::WindowMovedEventData(x, y));
		};

		m_window->GetWindowData().OnWindowResizeEvent += [=](unsigned int width, unsigned int height) {
			this->OnWindowResize(width, height);
			this->PropagateEventData(AIngine::Events::WindowResizeEvent::WindowResizeEventData(width, height));
		};
	}

	void Application::OnWindowClose()
	{
		//stop running
		m_isRunning = false;
	}

	void Application::OnWindowResize(unsigned int width, unsigned int height)
	{
		if (m_viewport)
			m_renderer->SetViewport();
	}

	void Application::OnViewportChanged(AIngine::Structures::RectangleI& viewport)
	{
		m_viewport->Set(viewport.GetPosition(), viewport.width, viewport.height, true);
		m_camera->SetZoom((float)m_window->GetWidth() / (float)m_world->GetBounds().y);
		m_renderer->SetViewport();
		CORE_INFO("Viewport Size Changed To ({0} | {1}) at Position ({2} | {3})", m_viewport->m_width, m_viewport->m_height, m_viewport->m_x, m_viewport->m_y);
	}

	std::vector<std::string> GetAvailableComponentNames()
	{
		std::vector<std::string> result = { "Sprite", "Physics", "Sound", "ParticleEmitter" };

		for (auto& it = ApplicationComponentNames.begin(); it != ApplicationComponentNames.end(); it++) {
			result.push_back(*it._Ptr);
		}

		return result;
	}
}