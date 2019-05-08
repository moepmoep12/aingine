#include "Physics.h"
#include "Application.h"

namespace AIngine {
	PhysicsComponent::PhysicsComponent(GameObject * owner) : Component(owner)
	{
		const Application& app = Application::Get();
		const b2World& physWorld = app.GetPhysicsWorld();
	}

	PhysicsComponent::~PhysicsComponent()
	{
	}

	void PhysicsComponent::OnUpdate(float deltatime)
	{
	}
}
