#include "Application.h"
#include "AIngine/Input.h"
#include "AIngine/Graphics.h"
#include "AIngine/Macros.h"
#include "AIngine/Script.h"
#include "AIngine/Sounds.h"
#include "AIngine/World.h"
#include "Editor/Editor.h"
#include "Events/ApplicationEvents.h"
#include "Rendering/Camera.h"
#include "Rendering/Font.h"
#include "Rendering/ParticleRenderer.h"
#include "Rendering/Renderer.h"
#include "Rendering/Viewport.h"
#include "Rendering/UIRenderer.h"
#include "Structures/SceneGraph.h"
#include "Util/Project.h"
#include "UI/ImGuiLayer.h"

namespace AIngine {

	Application* Application::s_instance = nullptr;
	double Application::FIXED_TIMESTEP = 1.0 / 60.0;

	Application::Application()
	{
		ASSERT(!s_instance, "Application already running");
		s_instance = this;

		// create window & register callbacks
		m_window = std::unique_ptr<Window>(Window::Create(m_windowConfig));
		RegisterCallbacks();

		// Create UI Layer
		m_imGuiLayer = new AIngine::UI::ImGuiLayer();
		PushOverlay(m_imGuiLayer);

#ifndef EDITOR
		// if the exe was moved after building we need to update the project dir
		std::filesystem::path actualpath = std::filesystem::canonical(AIngine::Util::Project::ExePath()).parent_path();
		std::filesystem::path p;
		if (std::filesystem::exists(AIngine::Util::Project::GetProjectDir()))
			p = std::filesystem::canonical(AIngine::Util::Project::GetProjectDir());
		if (p != actualpath)
			AIngine::Util::Project::SetProjectDir(actualpath.string());
#endif

		// create asset factories
		{
			m_assetRegistry.m_resourceFolderPath = AIngine::Util::Project::GetResourceDirectory();
#ifdef EDITOR
			m_assetRegistry.m_engineInstallPath = AIngine::Util::Project::GetEngineInstallDirectory();
#else
			// If we have a built binary, the engine resources are within the resource folder
			if (std::filesystem::exists(actualpath.string() + "\\Resources\\AIngine"))
				m_assetRegistry.m_engineInstallPath = AIngine::Util::Project::GetProjectDir() + "\\";
			else
				m_assetRegistry.m_engineInstallPath = AIngine::Util::Project::GetEngineInstallDirectory();
#endif
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

		// load icon
		AIngine::Assets::BitmapAsset* icon = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>("AIngine/textures/icon.png");
		m_window->SetIcon(icon->GetBitmap().GetWidth(), icon->GetBitmap().GetHeight(), icon->GetBitmap().GetData());

		// create sprite renderer
		m_spriteRenderer = new AIngine::Rendering::SpriteRenderer();

		//load basic white texture
		std::string path = std::string("AIngine/textures/White.png");
		AIngine::Assets::BitmapAsset* bitmap = m_assetRegistry.Load<AIngine::Assets::BitmapAsset>(path);

		// create Graphics API
		m_Graphics = new AIngine::Graphics();

		// create viewport
		m_viewport = new AIngine::Rendering::Viewport(m_window->GetWidth(), m_window->GetHeight(), 0, 0, *m_window.get());

		// create camera
		m_camera = new AIngine::Rendering::Camera(*m_viewport, glm::vec2(m_bounds.y - m_bounds.x, m_bounds.z - m_bounds.w));

		// create particle renderer
		m_particleRenderer = new AIngine::Rendering::ParticleRenderer();

		// create UI Renderer
		m_uiRenderer = new AIngine::Rendering::UIRenderer();
	}

	static float s_currentFrame = 0.0f;
	static float s_lastFrame = 0.0f;
	static float s_accumulated = 0.0f;
	static const float MAX_TIMESTEP = 0.2;

	void Application::Run()
	{
		// create game world
		m_world = new World(m_bounds, m_gravity);
		PushLayer(m_world);

		//create Sound API
		PushLayer(new SoundEngine(m_window->GetWin32Window()));

		// create Editor if its an editor build
#ifdef EDITOR
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
		m_window->SetFullScreen(m_windowConfig.startFullScreen);
		AIngine::Structures::Rectangle viewportRect = AIngine::Structures::RectangleI(m_window->GetX(), 0, m_window->GetWidth(), m_window->GetHeight());
		OnViewportChanged(viewportRect);
		// we're in release, load default scene
		LoadScene(0);
		OnEnterPlayMode();
#endif

		m_spriteRenderer->initRenderData();

		CORE_INFO("App is running!");

		m_isRunning = true;

		glfwSetTime(0);

		// the game loop
		while (m_isRunning)
		{
			// calc delta time
			s_currentFrame = (float)glfwGetTime();
			m_deltaTime = (s_currentFrame - s_lastFrame);
			s_lastFrame = s_currentFrame;
			if (m_deltaTime > MAX_TIMESTEP) m_deltaTime = MAX_TIMESTEP;
			s_accumulated += m_deltaTime;

			// handle user input
			m_window->PollInput();

			if (m_usesFixedTimeStep) {
				while (s_accumulated >= FIXED_TIMESTEP) {
					if ((AIngine::Editor::Editor::IsGameRunning()))
						// update game
						OnAppUpdate();

					// update logic
					for (AIngine::Structures::Layer* layer : m_layerStack)
						layer->OnUpdate(FIXED_TIMESTEP * AppSpeedMulitplier);

					s_accumulated -= FIXED_TIMESTEP;
				}
			}
			else {
				if ((AIngine::Editor::Editor::IsGameRunning()))
					// update game
					OnAppUpdate();

				// update logic
				for (AIngine::Structures::Layer* layer : m_layerStack)
					layer->OnUpdate(GetDeltaTimeScaled());
			}

			/// RENDERING 
			if (bRender) {
				// scene rendering
				m_spriteRenderer->Traverse(&m_world->GetSceneGraph().GetRoot());
				m_particleRenderer->Traverse(&m_world->GetSceneGraph().GetRoot());
				m_uiRenderer->Traverse(&m_world->GetSceneGraph().GetRoot());
			}

			// ui rendering
			m_imGuiLayer->OnBegin();
			for (AIngine::Structures::Layer* layer : m_layerStack) {
				layer->OnImGuiRender();
			}
			// finish UI rendering
			m_imGuiLayer->OnEnd();
			// render debug stuff last
			m_Graphics->Flush();

			// finish the frame
			m_window->OnUpdate();

			// Scene loading after updating the scenegraph
			if (m_wantsLoadLevel) {
				LoadScene(m_sceneToLoadIndex);
				m_wantsLoadLevel = false;
			}
		}

		CORE_INFO("Shutting App down...");

		//OnLeavePlayMode();

		// Clean up
		m_window = NULL;
		delete m_spriteRenderer;
		delete m_camera;
		delete m_Graphics;
		delete m_viewport;
		delete m_particleRenderer;
		delete m_uiRenderer;
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
	}

	void Application::OnLeavePlayMode()
	{
		PropagateEventData(AIngine::Events::ExitPlayModeEventData());
		OnAppShutDown();
	}

	float Application::GetDeltaTimeScaled()
	{
		return s_instance->GetDeltaTime() * s_instance->AppSpeedMulitplier;
	}

	float Application::GetDeltaTimeReal()
	{
		return s_instance->m_deltaTime;
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

	float Application::GetDeltaTime() const
	{
		return m_usesFixedTimeStep ? FIXED_TIMESTEP : m_deltaTime;
	}

	void Application::LoadScene(int index)
	{
		if (s_instance) {
			if (s_instance->m_wantsLoadLevel) {
				s_instance->m_wantsLoadLevel = false;
				std::vector<AIngine::Editor::Scene> scenes = AIngine::Editor::Editor::LoadBuildScenes();
				if (index < scenes.size()) {
					// propagate exiplaymodeEvent
					if (s_instance->m_editor)
						AIngine::Editor::Editor::SetIsInPlayMode(false);
					else {
						s_instance->PropagateEventData(AIngine::Events::ExitPlayModeEventData());
					}
					AIngine::Editor::Editor::ResetSceneGraph(s_instance);
					s_instance->OnLoadLevelStart(index);
					AIngine::Editor::Editor::LoadScene(AIngine::Util::Project::GetResourceDirectory() + scenes[index].Path);
					if (IsRunning()) s_instance->PropagateEventData(AIngine::Events::EnterPlayModeEventData());
					s_instance->OnLoadLevelFinished(index);
				}
			}
			else {
				s_instance->m_wantsLoadLevel = true;
				s_instance->m_sceneToLoadIndex = index;
			}
		}
	}

	void Application::UseFixedTimeStep(bool use)
	{
		s_instance->m_usesFixedTimeStep = use;
		s_instance->m_window->SetVSync(!use);
	}

	bool Application::IsRunning()
	{
		return AIngine::Editor::Editor::IsGameRunning();
	}

	void Application::ShutDown()
	{
		m_window->Close();
	}

	void Application::RegisterCallbacks()
	{
		m_window->GetWindowData().OnKeyPressedEvent += [=](AIngine::KeyCode key) {
			this->PropagateEventData(AIngine::Events::KeyPressedEvent::KeyPressedEventData(key, 1));
		};

		m_window->GetWindowData().OnKeyReleasedEvent += [=](AIngine::KeyCode key) {
			this->PropagateEventData(AIngine::Events::KeyReleasedEvent::KeyReleasedEventData(key));
		};

		m_window->GetWindowData().OnKeyTypedEvent += [=](AIngine::KeyCode key) {
			this->PropagateEventData(AIngine::Events::KeyTypedEvent::KeyTypedEventData(key));
		};

		m_window->GetWindowData().OnMouseButtonPressedEvent += [=](MouseButton button) {
			this->PropagateEventData(AIngine::Events::MouseButtonPressedEvent::MouseButtonPressedEventData(button));
		};

		m_window->GetWindowData().OnMouseButtonReleasedEvent += [=](MouseButton button) {
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
		m_isRunning = false;
	}

	void Application::OnWindowResize(unsigned int width, unsigned int height)
	{
		if (m_viewport && !m_window->IsFullScreen())
			m_spriteRenderer->SetViewport();
	}

	void Application::OnViewportChanged(AIngine::Structures::RectangleI& viewport)
	{
		m_viewport->Set(viewport.GetPosition(), viewport.width, viewport.height, true);
		m_camera->SetZoom((float)m_window->GetWidth() / (float)m_world->GetBounds().y);
		m_spriteRenderer->SetViewport();
		PropagateEventData(AIngine::Events::ViewportChangedEvent::ViewportChangedEventData(viewport));
		//CORE_INFO("Viewport Size Changed To ({0} | {1}) at Position ({2} | {3})", m_viewport->m_width, m_viewport->m_height, m_viewport->m_x, m_viewport->m_y);
	}

	std::vector<std::string> GetAvailableComponentNames()
	{
		std::vector<std::string> result = { "Sprite", "Physics", "Sound", "ParticleEmitter", "Canvas", "Button", "Image", "Text", "CheckBox", "Slider", "XCSAgentSupervisor" };

		for (auto& it = ApplicationComponentNames.begin(); it != ApplicationComponentNames.end(); it++) {
			result.push_back(*it._Ptr);
		}

		return result;
	}
}