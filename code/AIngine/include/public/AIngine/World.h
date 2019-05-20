#pragma once

#include "Structures/Layer.h"
#include "Structures/SceneGraph.h"
#include "Rendering/PhysicsRenderer.h"
#include <Box2D/Box2D.h>
#include <glm/glm.hpp>
#include <string>

namespace AIngine {
	class Application;
	class GameObject;
}

namespace AIngine::Editor {
	class Editor;
}

namespace AIngine {
	class World : public Layer {

		friend class AIngine::Application;
		friend class AIngine::Editor::Editor;
		friend class PhysicsComponent;

	public:
		static GameObject* const SpawnObject(const std::string& name = std::string("GameObject"), GameObject* parent = nullptr, const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f), const float rotation = 0.0f);
		static void DestroyObject(GameObject& gameobject);
		static inline const glm::vec4& GetBounds() { if (s_instance) return s_instance->m_bounds; else return glm::vec4(1.0); }
		static inline const SceneGraph& GetSceneGraph() { return *s_instance->m_sceneGraph; }
		static inline const b2World& GetPhysicsWorld() { return *s_instance->m_physicsWorld; }
		static inline b2Body* CreateBody(const b2BodyDef& bodydef) { if (s_instance) return s_instance->m_physicsWorld->CreateBody(&bodydef); else return nullptr; }

		// inherited via Layer
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float delta) override;
		virtual void OnEvent(AIngine::Events::Event& e) override;
		virtual void OnImGuiRender() override;

	private:
		static World* s_instance;
		SceneGraph* m_sceneGraph;
		b2World* m_physicsWorld;
		glm::vec4 m_bounds;
		glm::vec2 m_gravity;
		AIngine::Rendering::PhysicsRenderer* m_physRenderer;

		// created via Application
	private:
		World(const glm::vec4& bounds, const glm::vec2& gravity);
		World(float bottomLeft, float bottomRight, float topLeft, float topRight, const glm::vec2& gravity);
		virtual ~World() override;

		void CreateWorldBounds();
	};
}