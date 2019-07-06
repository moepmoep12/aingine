#pragma once

#include "Structures/Layer.h"

#include <glm/glm.hpp>
#include <string>

class b2World;
class b2Body;
struct b2BodyDef;

namespace AIngine {
	class Application;
	class GameObject;

	namespace Physics {
		class PhysicsComponent;
		class ContactListener;
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

	class World final : public AIngine::Structures::Layer {

		friend class AIngine::Editor::Editor;
		friend class AIngine::Physics::PhysicsComponent;

	public:
		World(const glm::vec4& bounds, const glm::vec2& gravity);
		World(float bottomLeft, float bottomRight, float topLeft, float topRight, const glm::vec2& gravity);
		virtual ~World() override;

		static GameObject* const SpawnObject(const std::string& name = std::string("GameObject"), GameObject* parent = nullptr, const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f), const float rotation = 0.0f);
		static GameObject* const GetGameObject(const std::string& name);
		static void DestroyObject(GameObject& gameobject);
		static inline glm::vec4 GetBounds() { if (s_instance) return s_instance->m_bounds; else return glm::vec4(1.0); }
		static inline glm::vec2 GetGravity() { if (s_instance) return s_instance->m_gravity; else return glm::vec2(0); }
		static void SetGravity(const glm::vec2& gravity);
		static b2Body* CreateBody(const b2BodyDef& bodydef);
		static inline bool IsDebugPhysicsDrawn() { if (s_instance) return s_instance->m_isPhysicsDebugDrawn; else return false; }
		static void SetPhysicsDebugDrawActive(const bool& active);
		static const b2World& GetPhysicsWorld();

		AIngine::Structures::SceneGraph& GetSceneGraph();

		// inherited via Layer
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float delta) override;
		virtual void OnEvent(AIngine::Events::EventData& e) override;
		virtual void OnImGuiRender() override;

	private:
		static World* s_instance;
		AIngine::Structures::SceneGraph* m_sceneGraph;
		b2World* m_physicsWorld;
		glm::vec4 m_bounds;
		glm::vec2 m_gravity;
		AIngine::Rendering::PhysicsRenderer* m_physRenderer;
		Physics::ContactListener* m_contactListener;
		bool m_isPhysicsDebugDrawn = false;

	private:
		void CreateWorldBounds();
	};
}