#pragma once
#include "AIngine/Component.h"
#include <Box2D/Box2D.h>
#include <glm/glm.hpp>

namespace AIngine::Editor {
	class SceneGraphWidget;
}

namespace AIngine::Physics {

	enum PhysicsShape {
		e_Circle = 0,
		e_Box = 1,
		e_Polygon = 2
	};

	enum PhysicsBodyType {
		e_Static = 0,
		e_Dynamic = 1,
		e_Kinematic = 2
	};

	struct PhysicsProperties {

		PhysicsProperties() {
			friction = 0.2f;
			restitution = 0.0f;
			density = 0.0f;
		}

		float32 friction;
		float32 restitution;
		float32 density;
	};

	struct PhysicsBodyInformation {
		PhysicsBodyType type;
		PhysicsShape shape;
		float32 radius;
		float32 width;
		float32 height;
		glm::vec2 vertices[8];
	};


	class PhysicsComponent : public AIngine::Component {
	public:

		friend class AIngine::Editor::SceneGraphWidget;

	public:
		PhysicsComponent(GameObject* owner);
		virtual ~PhysicsComponent();

		// inherited via Component
		virtual void OnUpdate(float deltatime) override;
		virtual void SetActive(bool active) override;
		virtual void OnOwnerTransformChanged(const glm::vec2 newPosition, const glm::vec2& newScale, const float& newRotation) override;
		virtual void OnOwnerLocalPositionChanged(const glm::vec2& position) override;
		virtual void OnOwnerLocalRotationChanged(const float& rot) override;

		void CreateBody(b2BodyDef& bodydef, b2FixtureDef& fixtureDef);
		void CreateCircleBody(const PhysicsProperties& properties, PhysicsBodyType type, float radius);
		void CreateBoxBody(const PhysicsProperties& properties, PhysicsBodyType type, float width, float height);

		void ApplyForce(const glm::vec2& force, const glm::vec2& point);
		void ApplyForce(const b2Vec2& force, const b2Vec2& point);
		void ApplyForceToCenter(const glm::vec2& force);
		void ApplyForceToCenter(const b2Vec2& force);
		void ApplyLinearImpulseToCenter(const glm::vec2& impulse);
		void ApplyLinearImpulseToCenter(const b2Vec2& impulse);

		inline bool IsCollided() const { return m_collided; }
		inline void SetCollision(bool collided, b2Fixture* other) { m_collided = collided; m_otherCollided = other; }

		inline b2Fixture* GetOtherCollider() { return m_otherCollided; }
		inline const PhysicsProperties& GetProperties() const { return m_properties; }
		inline const PhysicsBodyInformation& GetBodyInformation() const { return m_bodyInformation; }

	private:
		b2Body* m_body;
		b2Fixture* m_otherCollided;
		bool m_collided = false;
		PhysicsProperties m_properties;
		PhysicsBodyInformation m_bodyInformation;
	};


	class ContactListener : public b2ContactListener {
	public:
		// Called when two fixtures begin to touch
		virtual void BeginContact(b2Contact* contact) override
		{
			void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
			if (bodyUserData)
				static_cast<PhysicsComponent*> (bodyUserData)->SetCollision(true, contact->GetFixtureB());

			bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
			if (bodyUserData)
				static_cast<PhysicsComponent*> (bodyUserData)->SetCollision(true, contact->GetFixtureA());
		}

		// Called when two fixtures cease to touch
		virtual void EndContact(b2Contact* contact) override
		{
			void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
			if (bodyUserData)
				static_cast<PhysicsComponent*> (bodyUserData)->SetCollision(false, nullptr);

			//check if fixture B was a ball
			bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
			if (bodyUserData)
				static_cast<PhysicsComponent*> (bodyUserData)->SetCollision(false, nullptr);
		}

	};
}