#include "PickUp.h"
#include "CrappyBird.h"

namespace CrappyBird {

	static const float pickUpRadius = 0.2f;

	// Constructor
	PickUp::PickUp()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void PickUp::OnStart()
	{
		// create event handler
		m_collisionHandler = AIngine::Events::EventHandler<void, PhysicsComponent*>(std::bind(&PickUp::OnCollision, this, std::placeholders::_1));
		// register callback
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin += m_collisionHandler;

		m_player = AIngine::World::GetGameObject("PlayerRocket")->GetComponent<Player>();
	}

	// End is called when gameplay ends for this script
	void PickUp::OnEnd()
	{
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin -= this->m_collisionHandler;
		GetOwner()->SetActive(false);
	}

	// Update is called once per frame
	void PickUp::Update(float delta)
	{
		if (m_player->IsGameOver) return;

		GetOwner()->Translate(glm::vec2(-CrappyBird::s_GameSpeed  *delta, 0), true);

		// the pickUp is now beyond the player
		if (GetOwner()->GetLocalPosition().x <= -pickUpRadius)
		{
			GetOwner()->SetActive(false);
		}
	}

	void PickUp::OnCollision(AIngine::Physics::PhysicsComponent * other)
	{
		if (other->GetOwner()->GetName() == "PlayerRocket") {
			m_player->AddEffect(std::move(m_Effect));
			m_Effect = nullptr;
			GetOwner()->SetActive(false);
		}
	}

	// Callback for events
	void PickUp::OnEventData(AIngine::Events::EventData & e)
	{
	}
}