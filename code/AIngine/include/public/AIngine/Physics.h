#pragma once
#include "AIngine/Component.h"
#include "Events/Event.h"

#include <glm/glm.hpp>

class b2Body;

namespace AIngine::Editor::Widget {
	class SceneGraphWidget;
	namespace Component {
		class PhysicsComponentWidget;
	}
}

namespace AIngine::Physics {

	enum { maxVertices = 8 };

	enum class PhysicsShape {
		e_Circle = 0,
		e_Box = 1,
		e_Polygon = 2,
		e_Edge = 3
	};

	enum class  PhysicsBodyType {
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

		float friction;
		float restitution;
		float density;
	};

	struct PhysicsBodyInformation {
		PhysicsBodyType type = PhysicsBodyType::e_Static;
		PhysicsShape shape = PhysicsShape::e_Circle;
		bool isTrigger = false;
		float radius = 0;
		float width = 0;
		float height = 0;
		glm::vec2 vertices[maxVertices];
		unsigned int verticesCount = 0;
	};


	struct ContactPoint {
		glm::vec2 WorldPoint;
		float NormalImpulse;
		float TangentImpulse;
	};

	class PhysicsComponent;

	struct Contact {
		PhysicsComponent* Other;
		glm::vec2 Normal;
		bool IsTouching;
		float Friction;
		float Restitution;
		float TangentSpeed;
		int PointCount;
		ContactPoint ContactPoints[2];
	};

	typedef AIngine::Events::EventHandler<void, Contact> OnCollisionHandler;
	typedef AIngine::Events::Event<void, Contact> CollisionEvent;

	class PhysicsComponent final : public AIngine::Component {
	public:


		friend class AIngine::Editor::Widget::SceneGraphWidget;
		friend class AIngine::Editor::Widget::Component::PhysicsComponentWidget;

	public:
		virtual ~PhysicsComponent();

		// inherited via Component
		virtual void OnUpdate(float deltatime) override;
		virtual void SetEnabled(bool active) override;
		virtual void OnOwnerTransformChanged(const glm::vec2 newPosition, const glm::vec2& newScale, const float& newRotation) override;
		virtual void OnOwnerLocalPositionChanged(const glm::vec2& position) override;
		virtual void OnOwnerLocalRotationChanged(const float& rot) override;

		void CreateCircleBody(const PhysicsProperties& properties, PhysicsBodyType type, float radius, bool isTrigger = false);
		void CreateBoxBody(const PhysicsProperties& properties, PhysicsBodyType type, float width, float height, bool isTrigger = false);
		void CreateEdgeBody(const PhysicsProperties& properties, PhysicsBodyType type, const glm::vec2& p1Offset, const glm::vec2& p2Offset, bool isTrigger = false);
		void CreatePolygonBody(const PhysicsProperties& properties, PhysicsBodyType type, const glm::vec2* vertices, unsigned int count, bool isTrigger = false);

		void AdjustBoxShape(float width, float height);
		void AdjustPolyShape(const glm::vec2* vertices, unsigned int count);

		void ApplyForce(const glm::vec2& force, const glm::vec2& point);
		void ApplyForceToCenter(const glm::vec2& force);
		void ApplyLinearImpulseToCenter(const glm::vec2& impulse);

		inline glm::vec2 GetOffset() const { return m_offset; }
		inline void SetOffset(const glm::vec2& offset) { m_offset = offset; }

		bool IsFixedRotation() const;
		void SetFixedRotation(bool fixed);

		inline const PhysicsProperties& GetProperties() const { return m_properties; }
		inline const PhysicsBodyInformation& GetBodyInformation() const { return m_bodyInformation; }
		const Contact* const GetContact() const;

		glm::vec2 GetVelocity() const;
		float GetInertia() const;
		inline bool IsTouching() const { return m_bIsTouching; }

	public:
		CollisionEvent OnCollisionBegin;
		CollisionEvent OnCollisionEnd;

	protected:
		virtual Component* Copy(GameObject* const owner) const override;

	private:
		friend class ContactListener;
		bool m_bIsTouching = false;
		Contact m_contact;
		b2Body* m_body = nullptr;
		PhysicsProperties m_properties;
		PhysicsBodyInformation m_bodyInformation;
		glm::vec2 m_offset = glm::vec2(0);
	};


}