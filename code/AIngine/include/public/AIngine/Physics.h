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

		void CreateBody(b2BodyDef& bodydef, b2FixtureDef& fixtureDef);

		void ApplyForce(const glm::vec2& force, const glm::vec2& point);
		void ApplyForce(const b2Vec2& force, const b2Vec2& point);
		void ApplyForceToCenter(const glm::vec2& force);
		void ApplyForceToCenter(const b2Vec2& force);
		void ApplyLinearImpulseToCenter(const glm::vec2& impulse);
		void ApplyLinearImpulseToCenter(const b2Vec2& impulse);

		void UpdateTransform();

	private:
		b2Body* m_body;

	};
}