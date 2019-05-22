#pragma once
#include "AIngine/Component.h"
#include <Box2D/Box2D.h>
#include <glm/glm.hpp>

namespace AIngine::Editor {
	class SceneGraphWidget;
}

namespace AIngine {

	class PhysicsComponent : public AIngine::Component {

		friend class AIngine::Editor::SceneGraphWidget;

	public:
		PhysicsComponent(GameObject* owner);
		virtual ~PhysicsComponent();

		// inherited via Component
		virtual void OnUpdate(float deltatime) override;
		virtual void SetActive(bool active) override;
		virtual void OnOwnerLocalPositionChanged(const glm::vec2& position) override;
		virtual void OnOwnerLocalRotationChanged(const float& rot) override;

		void CreateBody(b2BodyDef& bodydef, b2FixtureDef& fixtureDef);

		void ApplyForce(const glm::vec2& force, const glm::vec2& point);
		void ApplyForce(const b2Vec2& force, const b2Vec2& point);
		void ApplyForceToCenter(const glm::vec2& force);
		void ApplyForceToCenter(const b2Vec2& force);
		void ApplyLinearImpulseToCenter(const glm::vec2& impulse);
		void ApplyLinearImpulseToCenter(const b2Vec2& impulse);

		inline bool IsCollided() const { return m_collided; }
		inline void SetCollision(bool collided, b2Fixture* other) { m_collided = collided; m_otherCollided = other; }

		inline b2Fixture* GetOtherCollider() { return m_otherCollided; }

	private:
		b2Body* m_body;
		b2Fixture* m_otherCollided;
		bool m_collided = false;

	};
}