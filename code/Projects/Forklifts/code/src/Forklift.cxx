#include "Forklift.h"
#include "Node.h"

namespace Forklifts {

	// Constructor
	Forklift::Forklift()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void Forklift::OnStart()
	{
		OnCollisionEventHandler = AIngine::Events::EventHandler<void, AIngine::Physics::Contact>(std::bind(&Forklift::OnCollision, this, std::placeholders::_1));
		GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>()->OnCollisionBegin += OnCollisionEventHandler;
		GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>()->SetEnabled(true);
	}

	// End is called when gameplay ends for this script
	void Forklift::OnEnd()
	{
		GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>()->OnCollisionBegin -= OnCollisionEventHandler;
	}

	// Update is called once per frame
	void Forklift::Update(float delta)
	{
	}

	// Callback for events
	void Forklift::OnEventData(AIngine::Events::EventData & e)
	{
	}


	void Forklift::OnCollision(AIngine::Physics::Contact contact)
	{
		if (contact.Other && contact.Other->GetComponent<Node>())
		{
			m_nodeReached = true;
		}
	}
}