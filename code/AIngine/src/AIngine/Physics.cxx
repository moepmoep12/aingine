#include "AIngine/Physics.h"
#include "Application.h"
#include "AIngine/Constants.h"

namespace AIngine {
	PhysicsComponent::PhysicsComponent(GameObject * owner) : Component(owner)
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
			m_body->SetActive(active);
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

	void PhysicsComponent::CreateBody(b2BodyDef & bodydef, b2FixtureDef& fixtureDef)
	{
		glm::vec2 worldPos = m_owner->GetWorldPosition();
		bodydef.position.Set(worldPos.x, worldPos.y);
		bodydef.angle = m_owner->GetWorldRotation();
		m_body = AIngine::World::CreateBody(bodydef);
		m_body->CreateFixture(&fixtureDef);
		m_body->SetUserData(this);
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

}
