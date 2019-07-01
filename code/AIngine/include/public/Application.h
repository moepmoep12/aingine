#pragma once

#include "Window.h"
#include "Events/Event.h"
#include "Structures/LayerStack.h"
#include "Assets/Assets.h"

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace AIngine {

	class GameObject;
	class World;
	class Graphics;

	// to be defined by the actual Application
	extern Application* CreateApplication();
	// Components defined by the Application should be defined here, this includes Scripts
	// This is used by the auto generated Scripting API
	extern std::vector<std::string> ApplicationComponentNames;
	// Callback method for adding a User-defined compononent inside the editor
	// This is used by the auto generated Scripting API
	extern void OnAddComponent(GameObject* obj, int index);
	// Returns a list of all Components (including Components defined by the Engine)
	std::vector<std::string> GetAvailableComponentNames();

	// Forward Declarations
	namespace Editor {
		class Editor;
	}

	namespace Rendering {
		class SpriteRenderer;
		class ParticleRenderer;
		class UIRenderer;
		class Camera;
		class Viewport;
	}

	namespace UI {
		class ImGuiLayer;
	}

	/* This is the base class for the running app. The app created with AIngine extends Application. */
	class Application {

		friend class Editor::Editor;

	public:
		Application();
		virtual ~Application() {}

		/* Starts the Application */
		void Run();

		/* Returns whether the game is currently running */
		static bool IsRunning();

		/* Returns the time it took to process the last frame in seconds */
		float GetDeltaTime();

		/* Adds a layer to the layerstack */
		void PushLayer(AIngine::Structures::Layer* layer);
		void PushOverlay(AIngine::Structures::Layer* overlay);

		/* Returns the currently running instance */
		inline static Application& Get() { return *s_instance; }
		/* Returns the window in which the Application is running */
		inline const Window& GetWindow() const { return *m_window; }
		/* Returns the viewport in which the game is running*/
		inline static const AIngine::Rendering::Viewport& GetViewport() { return *s_instance->m_viewport; }

		/* Loads a Scene with the given index. The index was specified in the build process */
		static void LoadScene(int index);

	protected:
		std::unique_ptr<Window> m_window = NULL;
		WindowConfig m_windowConfig;
		glm::vec2 m_gravity;
		glm::vec4 m_bounds;


		/* StartUp is called once when Playmode is entered */
		virtual void OnAppStartUp() = 0;
		/* ShutDown is called once before the scene was sthudown */
		virtual void OnAppShutDown() = 0;
		/* Update is called once per frame */
		virtual void OnAppUpdate() = 0;
		/* AppEvent propagates events */
		virtual void OnAppEvent(AIngine::Events::EventData& e) {}

		/* Shuts the App down. This is equal to closing the window. */
		void ShutDown();

	private:
		void RegisterCallbacks();
		// Callbacks
		void OnWindowClose();
		void OnWindowResize(unsigned int width, unsigned int height);
		void OnViewportChanged(AIngine::Structures::RectangleI&);
		void PropagateEventData(AIngine::Events::EventData& e);
		void OnEnterPlayMode();
		void OnLeavePlayMode();


	private:
		using LayerStack = AIngine::Structures::LayerStack;
		using SpriteRenderer = AIngine::Rendering::SpriteRenderer;
		using ParticleRenderer = AIngine::Rendering::ParticleRenderer;
		using UIRenderer = AIngine::Rendering::UIRenderer;
		using Viewport = AIngine::Rendering::Viewport;
		using ImGuiLayer = AIngine::UI::ImGuiLayer;
		using AssetRegistry = AIngine::Assets::AssetRegistry;
		using Editor = AIngine::Editor::Editor;
		using Camera = AIngine::Rendering::Camera;

		LayerStack m_layerStack;
		SpriteRenderer* m_spriteRenderer = nullptr;
		ParticleRenderer* m_particleRenderer = nullptr;
		UIRenderer* m_uiRenderer = nullptr;
		Viewport* m_viewport = nullptr;
		ImGuiLayer* m_imGuiLayer = nullptr;
		AssetRegistry m_assetRegistry;
		Editor* m_editor = nullptr;
		Graphics* m_Graphics = nullptr;
		Camera* m_camera = nullptr;
		World* m_world = nullptr;
		bool m_isRunning = false;
		bool m_wantsLoadLevel = false;
		int m_sceneToLoadIndex = 0;
		float m_deltaTime = 0.0f;

		static Application* s_instance;
	};

}