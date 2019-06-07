#include "AIngine/Physics.h"
#include "Application.h"
#include "AIngine/Constants.h"
#include "AIngine/GameObject.h"

namespace AIngine::Physics {
	PhysicsComponent::PhysicsComponent(GameObject * owner) : Component(owner), m_body(nullptr)
	{
	}

	PhysicsComponent::~PhysicsComponent()
	{
		if (m_body && AIngine::World::s_instance) {
			AIngine::World::s_instance->m_physicsWorld->DestroyBody(m_body);
		}
	}

	void PhysicsComponent::OnUpdate(float deltatime)
	{
		if (m_body && m_body->GetType() != b2_staticBody) {
			b2Vec2 pos = m_body->GetPosition();
			float rot = std::fmodf(m_body->GetAngle(), M_PI);

			m_owner->SetWorldPosition(glm::vec2(pos.x, pos.y), false);
			m_owner->SetWorldRotation(rot, false);
		}
	}

	void PhysicsComponent::SetActive(bool active)
	{
		Component::SetActive(active);

		if (m_body) {
			//m_body->SetActive(active);
			m_body->SetAwake(active);
		}
	}

	void PhysicsComponent::OnOwnerTransformChanged(const glm::vec2 newPosition, const glm::vec2 & newScale, const float & newRotation)
	{
		if (m_body && IsActive()) {
			m_body->SetAwake(false);
			glm::vec2 worldPos = m_owner->GetWorldPosition();
			m_body->SetTransform(b2Vec2(worldPos.x, worldPos.y), std::fmodf(m_owner->GetWorldRotation(), M_PI));
			m_body->SetAwake(true);
		}
	}

	void PhysicsComponent::OnOwnerLocalPositionChanged(const glm::vec2 & position)
	{
		if (m_body && IsActive()) {
			m_body->SetAwake(false);
			glm::vec2 worldPos = m_owner->GetWorldPosition();
			m_body->SetTransform(b2Vec2(worldPos.x, worldPos.y), std::fmodf(m_owner->GetWorldRotation(), M_PI));
			m_body->SetAwake(true);
		}
	}

	void PhysicsComponent::OnOwnerLocalRotationChanged(const float & rot)
	{
		if (m_body && IsActive()) {
			m_body->SetAwake(false);
			glm::vec2 worldPos = m_owner->GetWorldPosition();
			m_body->SetTransform(b2Vec2(worldPos.x, worldPos.y), std::fmodf(m_owner->GetWorldRotation(), M_PI));
			m_body->SetAwake(true);
		}
	}

	void PhysicsComponent::CreateCircleBody(const PhysicsProperties & properties, PhysicsBodyType type, float radius, bool isTrigger)
	{
		if (m_body) {
			AIngine::World::s_instance->m_physicsWorld->DestroyBody(m_body);
		}
		m_properties = properties;
		m_bodyInformation.type = type;
		m_bodyInformation.shape = PhysicsShape::e_Circle;
		m_bodyInformation.radius = radius;
		m_bodyInformation.isTrigger = isTrigger;

		b2FixtureDef fixtureDef;
		fixtureDef.density = properties.density;
		fixtureDef.friction = properties.friction;
		fixtureDef.restitution = properties.restitution;

		b2BodyDef bodyDef;
		if (type != PhysicsBodyType::e_Static) {
			if (type == PhysicsBodyType::e_Dynamic) {
				bodyDef.type = b2_dynamicBody;
			}
			else {
				bodyDef.type = b2_kinematicBody;
			}
		}
		glm::vec2 worldPos = m_owner->GetWorldPosition();
		bodyDef.position.Set(worldPos.x, worldPos.y);
		bodyDef.angle = m_owner->GetWorldRotation();

		b2CircleShape shape;
		shape.m_radius = radius;

		fixtureDef.shape = &shape;
		fixtureDef.isSensor = isTrigger;

		m_body = AIngine::World::CreateBody(bodyDef);
		m_body->CreateFixture(&fixtureDef);
		m_body->SetUserData(this);
	}

	void PhysicsComponent::CreateBoxBody(const PhysicsProperties & properties, PhysicsBodyType type, float width, float height, bool isTrigger)
	{
		if (m_body) {
			AIngine::World::s_instance->m_physicsWorld->DestroyBody(m_body);
		}
		m_properties = properties;
		m_bodyInformation.width = width;
		m_bodyInformation.height = height;
		m_bodyInformation.shape = PhysicsShape::e_Box;
		m_bodyInformation.type = type;
		m_bodyInformation.isTrigger = isTrigger;


		b2FixtureDef fixtureDef;
		fixtureDef.density = properties.density;
		fixtureDef.friction = properties.friction;
		fixtureDef.restitution = properties.restitution;

		b2BodyDef bodyDef;
		if (type != PhysicsBodyType::e_Static) {
			if (type == PhysicsBodyType::e_Dynamic) {
				bodyDef.type = b2_dynamicBody;
			}
			else {
				bodyDef.type = b2_kinematicBody;
			}
		}
		glm::vec2 worldPos = m_owner->GetWorldPosition();
		bodyDef.position.Set(worldPos.x, worldPos.y);
		bodyDef.angle = m_owner->GetWorldRotation();

		b2PolygonShape shape;
		shape.SetAsBox(width / 2.0f, height / 2.0f);

		fixtureDef.shape = &shape;
		fixtureDef.isSensor = isTrigger;

		m_body = AIngine::World::CreateBody(bodyDef);
		m_body->CreateFixture(&fixtureDef);
		m_body->SetUserData(this);
	}

	void PhysicsComponent::CreateEdgeBody(const PhysicsProperties & properties, PhysicsBodyType type, const glm::vec2 & p1Offset, const glm::vec2 & p2Offset, bool isTrigger)
	{
		if (m_body) {
			AIngine::World::s_instance->m_physicsWorld->DestroyBody(m_body);
		}
		m_properties = properties;
		m_bodyInformation.shape = PhysicsShape::e_Edge;
		m_bodyInformation.type = type;
		m_bodyInformation.isTrigger = isTrigger;


		b2FixtureDef fixtureDef;
		fixtureDef.density = properties.density;
		fixtureDef.friction = properties.friction;
		fixtureDef.restitution = properties.restitution;

		b2BodyDef bodyDef;
		if (type != PhysicsBodyType::e_Static) {
			if (type == PhysicsBodyType::e_Dynamic) {
				bodyDef.type = b2_dynamicBody;
			}
			else {
				bodyDef.type = b2_kinematicBody;
			}
		}
		glm::vec2 worldPos = m_owner->GetWorldPosition();
		bodyDef.position.Set(worldPos.x, worldPos.y);
		bodyDef.angle = m_owner->GetWorldRotation();

		b2EdgeShape shape;
		shape.Set(b2Vec2(p1Offset.x, p1Offset.y), b2Vec2(p2Offset.x, p2Offset.y));

		fixtureDef.shape = &shape;
		fixtureDef.isSensor = isTrigger;

		m_body = AIngine::World::CreateBody(bodyDef);
		m_body->CreateFixture(&fixtureDef);
		m_body->SetUserData(this);
	}

	PhysicsComponent* PhysicsComponent::GetOtherCollider() {
		if (m_otherCollided) {
			return static_cast<PhysicsComponent*>(m_otherCollided->GetBody()->GetUserData());
		}
		return nullptr;
	}

	glm::vec2 PhysicsComponent::GetVelocity() const {
		b2Vec2 vel = m_body->GetLinearVelocity();
		return glm::vec2(vel.x, vel.y);
	}


	void PhysicsComponent::ApplyForce(const glm::vec2 & force, const glm::vec2 & point)
	{
		ApplyForce(b2Vec2(force.x, force.y), b2Vec2(point.x, point.y));
	}
	void PhysicsComponent::ApplyForce(const b2Vec2 & force, const b2Vec2 & point)
	{
		m_body->ApplyForce(force, point, true);
	}
	void PhysicsComponent::ApplyForceToCenter(const glm::vec2 & force)
	{
		ApplyForceToCenter(b2Vec2(force.x, force.y));
	}
	void PhysicsComponent::ApplyForceToCenter(const b2Vec2 & force)
	{
		m_body->ApplyForceToCenter(force, true);
	}
	void PhysicsComponent::ApplyLinearImpulseToCenter(const glm::vec2 & impulse)
	{
		ApplyLinearImpulseToCenter(b2Vec2(impulse.x, impulse.y));
	}
	void PhysicsComponent::ApplyLinearImpulseToCenter(const b2Vec2 & impulse)
	{
		m_body->ApplyLinearImpulseToCenter(impulse, true);
	}

	void PhysicsComponent::SetCollision(bool collided, b2Fixture * other)
	{
		m_collided = collided;
		m_otherCollided = other;
		if (collided)
			OnCollisionBegin(GetOtherCollider());
		else OnCollisionEnd(GetOtherCollider());
	}

}


// initialize EventHandler for OnCollionEvent
int AIngine::Events::EventHandler<void, AIngine::Physics::PhysicsComponent*>::counter = 0;