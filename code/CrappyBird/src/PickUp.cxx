#include "PickUp.h"
#include "AIngine/Core.h"
#include "CrappyBird.h"

namespace CrappyBird {
	PickUp::PickUp(AIngine::GameObject * owner)
		: Script(owner)
	{
		SetName(typeid(*this).name());
	}

	void PickUp::OnStart()
	{
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin += std::bind(&PickUp::OnCollision, this, std::placeholders::_1);
	}

	void PickUp::OnCollision(AIngine::Physics::PhysicsComponent * other)
	{
		if (other->GetOwner()->GetName() == "PlayerRocket") {
			GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin -= std::bind(&PickUp::OnCollision, this, std::placeholders::_1);
			GetOwner()->GetComponent<PhysicsComponent>()->SetActive(false);
			this->OnPickUp();
		}
	}

	void SpeedPickUp::OnPickUp()
	{
		Active = true;
		CrappyBird::s_GameSpeed *= 5.0f;
		GetOwner()->GetComponent<Sprite>()->SetActive(false);
	}

	void SpeedPickUp::Update(float deltatime)
	{
		if (Active) {
			Duration -= deltatime;
			if (Duration <= 0) {
				Active = false;
				CrappyBird::s_GameSpeed /= 5.0f;
				SetActive(false);
			}
		}
	}
}