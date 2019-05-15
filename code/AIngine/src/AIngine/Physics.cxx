#include "AIngine/Physics.h"
#include "Application.h"

namespace AIngine {
	PhysicsComponent::PhysicsComponent(GameObject * owner) : Component(owner)
	{
		//const Application& app = Application::Get();
		//const b2World& physWorld = app.GetPhysicsWorld();

		//b2BodyDef bodydef;
		//bodydef.type = b2_dynamicBody;


		//b2CircleShape shape;
		//shape.m_radius = 1.0;

		//b2FixtureDef fixturedef;
		//fixturedef.shape = &shape;
		//fixturedef.density = 1.0;
		//m_body->CreateFixture(&fixturedef);
	}

	PhysicsComponent::~PhysicsComponent()
	{
	}

	void PhysicsComponent::OnUpdate(float deltatime)
	{
		if (m_body) {
			b2Vec2 pos = m_body->GetPosition();
			float rot = m_body->GetAngle();

			m_owner->SetLocalPosition(glm::vec2(pos.x, pos.y));
			m_owner->SetRotation(rot);
		}
	}
	void PhysicsComponent::CreateBody(b2BodyDef & bodydef, b2FixtureDef& fixtureDef)
	{
		glm::vec2 worldPos = m_owner->GetWorldPosition();
		bodydef.position.Set(worldPos.x, worldPos.y);
		m_body = AIngine::World::CreateBody(bodydef);
		m_body->CreateFixture(&fixtureDef);
	}
}
