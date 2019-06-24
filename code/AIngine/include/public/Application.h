#pragma once

#include <memory>
#include "Window.h"
#include "Events/Event.h"
#include "Events/ApplicationEvents.h"
#include "UI/ImGuiLayer.h"
#include "Structures/LayerStack.h"
#include "Assets/Assets.h"
#include "Box2D/Box2D.h"
#include "Rendering/Camera.h"
#include "Rendering/Viewport.h"
#include "AIngine/World.h"
#include "AIngine/Graphics.h"

namespace AIngine::Editor {
}

namespace AIngine {

	// All available Components should be listed by the Application
	extern std::vector<std::string> ApplicationComponentNames;

	extern void OnAddComponent(GameObject* obj,int index);

	std::vector<std::string> GetAvailableComponentNames();

	namespace Editor {
		class Editor;
	}

	namespace Rendering {
		class SpriteRenderer;
		class ParticleRenderer;
	}

	class Application {

		friend class Editor::Editor;


	public:
		Application();
		virtual ~Application();

		void Run();
		//void OnEvent(AIngine::Events::Event& e);
		void PushLayer(AIngine::Structures::Layer* layer);
		void PushOverlay(AIngine::Structures::Layer* overlay);
		float GetDeltaTime();
		bool IsAnyUiElementHovered() const;


		inline static Application& Get() { return *s_instance; }
		inline const Window& GetWindow() const { return *m_window; }

		static void LoadScene(int index);

		static bool IsRunning();
		static std::string GetResourceDirectory();
	protected:

		WindowConfig m_windowConfig;
		glm::vec2 m_gravity;
		glm::vec4 m_bounds;
		AIngine::Rendering::Camera* m_camera;
		World* m_world;
		std::unique_ptr<Window> m_window;

		virtual void OnAppStartUp() = 0;
		virtual void OnAppShutDown() = 0;
		virtual void OnAppUpdate() = 0;
		virtual void OnAppEvent(AIngine::Events::EventData& e) {}


		void ShutDown();
		static std::string GetInstallPath();

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
		AIngine::Editor::Editor* m_editor;
		bool m_isRunning = false;
		bool m_wantsLoadLevel = false;
		int m_sceneToLoadIndex = 0;
		AIngine::Structures::LayerStack m_layerStack;
		AIngine::UI::ImGuiLayer* m_imGuiLayer;
		AIngine::Assets::AssetRegistry m_assetRegistry;
		AIngine::Rendering::SpriteRenderer* m_renderer;
		AIngine::Rendering::ParticleRenderer* m_particleRenderer;
		AIngine::Rendering::Viewport* m_viewport;
		static Application* s_instance;
		float m_deltaTime = 0.0f;
		Graphics* m_Graphics;
	};

	// to be defined by the actual Application
	Application* CreateApplication();
}