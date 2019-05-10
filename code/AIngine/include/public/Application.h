#pragma once

#include <memory>
#include "Window.h"
#include "Events/Event.h"
#include "Events/ApplicationEvents.h"
#include "UI/ImGuiLayer.h"
#include "LayerStack.h"
#include "Assets.h"
#include "Rendering/Renderer.h"
#include "SceneGraph.h"
#include "Box2D/Box2D.h"
#include "Rendering/Camera.h"
#include "Rendering/Viewport.h"

namespace AIngine {

	class Application {

	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(AIngine::Events::Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		float GetDeltaTime();

		inline static Application& Get() { return *s_instance; }
		inline const Window& GetWindow() const { return *m_window; }
		inline const b2World& GetPhysicsWorld() const { return *m_physicsWorld; }
		inline  AIngine::Rendering::Camera& GetCamera() const { return *m_camera; }
		inline const AIngine::Rendering::Viewport& GetViewport() const { return *m_viewport; }
		inline  AIngine::Assets::AssetRegistry& GetAssetRegistry() { return m_assetRegistry; }

	protected:

		SceneGraph* m_sceneGraph;
		b2World* m_physicsWorld;
		b2Vec2 m_gravity;
		AIngine::Rendering::Camera* m_camera;

		virtual void OnAppStartUp() = 0;
		virtual void OnAppShutDown() = 0;
		virtual void OnAppUpdate() = 0;

	private:
		bool OnWindowClose(AIngine::Events::WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_window;
		bool m_isRunning = false;
		LayerStack m_layerStack;
		AIngine::UI::ImGuiLayer* m_imGuiLayer;
		AIngine::Assets::AssetRegistry m_assetRegistry;
		AIngine::Rendering::SpriteRenderer* m_renderer;
		AIngine::Rendering::Viewport* m_viewport;
		static Application* s_instance;
		float m_deltaTime = 0.0f;


	};

	// to be defined by the actual Application
	Application* CreateApplication();
}