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
#include "Debug/DebugDraw.h"

namespace AIngine::Editor {
}

namespace AIngine {

	namespace Editor {
		class Editor;
	}

	namespace Rendering{
		class SpriteRenderer;
	}

	class Application {

		friend class Editor::Editor;


	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(AIngine::Events::Event& e);
		void PushLayer(AIngine::Structures::Layer* layer);
		void PushOverlay(AIngine::Structures::Layer* overlay);
		float GetDeltaTime();
		bool IsAnyUiElementHovered() const;


		inline static Application& Get() { return *s_instance; }
		inline const Window& GetWindow() const { return *m_window; }

	protected:

		WindowConfig m_windowConfig;
		glm::vec2 m_gravity;
		glm::vec4 m_bounds;
		AIngine::Rendering::Camera* m_camera;
		World* m_world;

		virtual void OnAppStartUp() = 0;
		virtual void OnAppShutDown() = 0;
		virtual void OnAppUpdate() = 0;
		virtual void OnAppEvent(AIngine::Events::Event& e) {}

	private:
		bool OnWindowClose(AIngine::Events::WindowCloseEvent& e);
		bool OnWindowResize(AIngine::Events::WindowResizeEvent& e);
		bool OnViewportChanged(AIngine::Events::ViewportChangedEvent& e);

	private:
		std::unique_ptr<Window> m_window;
		AIngine::Editor::Editor* m_editor;
		bool m_isRunning = false;
		AIngine::Structures::LayerStack m_layerStack;
		AIngine::UI::ImGuiLayer* m_imGuiLayer;
		AIngine::Assets::AssetRegistry m_assetRegistry;
		AIngine::Rendering::SpriteRenderer* m_renderer;
		AIngine::Rendering::Viewport* m_viewport;
		static Application* s_instance;
		float m_deltaTime = 0.0f;
		AIngine::Debug::DebugDraw* m_debugDraw;
	};

	// to be defined by the actual Application
	Application* CreateApplication();
}