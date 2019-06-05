#pragma once

#include "Structures/Layer.h"

#include <Box2D/Box2D.h>
#include <glm/glm.hpp>
#include <string>

namespace AIngine {
	class Application;
	class GameObject;

	namespace Physics {
		class PhysicsComponent;
		class ContactListener;
	}

	namespace Editor {
		class Editor;
	}

	namespace Structures {
		class SceneGraph;
	}

	namespace Rendering {
		class PhysicsRenderer;
	}
}

namespace AIngine {
	class World : public AIngine::Structures::Layer {

		friend class AIngine::Editor::Editor;
		friend class AIngine::Physics::PhysicsComponent;

	public:
		World(const glm::vec4& bounds, const glm::vec2& gravity);
		World(float bottomLeft, float bottomRight, float topLeft, float topRight, const glm::vec2& gravity);
		virtual ~World() override;

		static GameObject* const SpawnObject(const std::string& name = std::string("GameObject"), GameObject* parent = nullptr, const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f), const float rotation = 0.0f);
		static void DestroyObject(GameObject& gameobject);
		static inline const glm::vec4& GetBounds() { if (s_instance) return s_instance->m_bounds; else return glm::vec4(1.0); }
		static inline const b2World& GetPhysicsWorld() { return *s_instance->m_physicsWorld; }
		static inline b2Body* CreateBody(const b2BodyDef& bodydef) { if (s_instance) return s_instance->m_physicsWorld->CreateBody(&bodydef); else return nullptr; }

		static void SetPhysicsDebugDrawActive(const bool& active);

		AIngine::Structures::SceneGraph& GetSceneGraph();

		// inherited via Layer
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float delta) override;
		virtual void OnEvent(AIngine::Events::Event& e) override;
		virtual void OnImGuiRender() override;

	private:
		static World* s_instance;
		AIngine::Structures::SceneGraph* m_sceneGraph;
		b2World* m_physicsWorld;
		glm::vec4 m_bounds;
		glm::vec2 m_gravity;
		AIngine::Rendering::PhysicsRenderer* m_physRenderer;
		Physics::ContactListener* m_contactListener;
		bool m_isPhysicsDebugDrawn;

	private:
		void CreateWorldBounds();
	};
}