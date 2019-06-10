#pragma once
#include "AIngine/Component.h"
#include "Events/Event.h"

#include <Box2D/Box2D.h>
#include <glm/glm.hpp>

namespace AIngine::Editor {
	class SceneGraphWidget;
	class PhysicsComponentWidget;
}

namespace AIngine::Physics {

	enum { maxVertices = 8 };

	enum PhysicsShape {
		e_Circle = 0,
		e_Box = 1,
		e_Polygon = 2,
		e_Edge = 3
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
			density = 1.0f;
		}

		float32 friction;
		float32 restitution;
		float32 density;
	};

	struct PhysicsBodyInformation {
		PhysicsBodyType type = PhysicsBodyType::e_Static;
		PhysicsShape shape = PhysicsShape::e_Circle;
		bool isTrigger = false;
		float32 radius = 0;
		float32 width = 0;
		float32 height = 0;
		glm::vec2 vertices[maxVertices];
		unsigned int verticesCount = 0;
	};


	class PhysicsComponent : public AIngine::Component {
	public:

		class CollisionEvent : public AIngine::Events::Event<void, PhysicsComponent*> {

		};

		friend class AIngine::Editor::SceneGraphWidget;
		friend class AIngine::Editor::PhysicsComponentWidget;

	public:
		PhysicsComponent(GameObject* owner);
		virtual ~PhysicsComponent();

		// inherited via Component
		virtual void OnUpdate(float deltatime) override;
		virtual void SetActive(bool active) override;
		virtual void OnOwnerTransformChanged(const glm::vec2 newPosition, const glm::vec2& newScale, const float& newRotation) override;
		virtual void OnOwnerLocalPositionChanged(const glm::vec2& position) override;
		virtual void OnOwnerLocalRotationChanged(const float& rot) override;

		//void CreateBody(b2BodyDef& bodydef, b2FixtureDef& fixtureDef);
		void CreateCircleBody(const PhysicsProperties& properties, PhysicsBodyType type, float radius, bool isTrigger = false);
		void CreateBoxBody(const PhysicsProperties& properties, PhysicsBodyType type, float width, float height, bool isTrigger = false);
		void CreateEdgeBody(const PhysicsProperties& properties, PhysicsBodyType type, const glm::vec2& p1Offset, const glm::vec2& p2Offset, bool isTrigger = false);
		void CreatePolygonBody(const PhysicsProperties& properties, PhysicsBodyType type, const glm::vec2* vertices, unsigned int count, bool isTrigger = false);

		void ApplyForce(const glm::vec2& force, const glm::vec2& point);
		void ApplyForce(const b2Vec2& force, const b2Vec2& point);
		void ApplyForceToCenter(const glm::vec2& force);
		void ApplyForceToCenter(const b2Vec2& force);
		void ApplyLinearImpulseToCenter(const glm::vec2& impulse);
		void ApplyLinearImpulseToCenter(const b2Vec2& impulse);

		inline bool IsCollided() const { return m_collided; }
		void SetCollision(bool collided, b2Fixture* other);

		inline glm::vec2 GetOffset() const { return m_offset; }
		inline void SetOffset(const glm::vec2& offset) { m_offset = offset; }

		inline const PhysicsProperties& GetProperties() const { return m_properties; }
		inline const PhysicsBodyInformation& GetBodyInformation() const { return m_bodyInformation; }

		//query data

		glm::vec2 GetVelocity() const;

		PhysicsComponent* GetOtherCollider();

	public:
		CollisionEvent OnCollisionBegin;
		CollisionEvent OnCollisionEnd;

	private:
		b2Body* m_body;
		b2Fixture* m_otherCollided;
		bool m_collided = false;
		PhysicsProperties m_properties;
		PhysicsBodyInformation m_bodyInformation;
		glm::vec2 m_offset = glm::vec2(0);
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