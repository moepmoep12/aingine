#include "PickUp.h"
#include "CrappyBird.h"
#include "Player.h""

#include <random>
#include <time.h>

namespace CrappyBird {

	static const float pickUpRadius = 0.2f;

	PickUpFactory::PickUpFactory()
	{
		SetName(typeid(*this).name());
		srand(time(NULL));
	}

	void PickUpFactory::OnStart()
	{
		//m_newScreenHandler = AIngine::Events::EventHandler<void, AIngine::Structures::RectangleI&>(std::bind(&PickUpFactory::SpawnPickUpInArea, this, std::placeholders::_1));

		//AIngine::World::GetGameObject("PlayerRocket")->GetComponent<Player>()->OnEnterNewScreen += m_newScreenHandler;
	}

	void PickUpFactory::OnEnd()
	{
		//AIngine::World::GetGameObject("PlayerRocket")->GetComponent<Player>()->OnEnterNewScreen -= m_newScreenHandler;
	}

	void PickUpFactory::SpawnPickUp(const glm::vec2 & pos)
	{
		GameObject* availableObj = GetAvailableGameObject();
		if (availableObj) {
			availableObj->SetLocalPosition(pos);
			AddEffect(*availableObj->GetComponent<PickUp>());
			availableObj->SetActive(true);
		}
	}

	GameObject* PickUpFactory::GetAvailableGameObject()
	{
		for (auto& child : GetOwner()->GetChildren()) {
			if (!child->IsActive())
				return child;
		}
		return nullptr;
	}

	void PickUpFactory::AddEffect(PickUp & obj)
	{
		int value = rand() % 2;

		switch (value) {
		case 0:
			obj.m_Effect = SpeedEffect();
			break;

		case 1:
			obj.m_Effect = SlowEffect();
			break;


		}

		Sprite* sprite = obj.GetOwner()->GetComponent<Sprite>();
		sprite->SetTexture(obj.m_Effect.Texture);
		sprite->SetColor(obj.m_Effect.Color);
		obj.GetOwner()->SetRotation(obj.m_Effect.Rotation);
	}

	/******************************** PICKUP ******************************************/


	PickUp::PickUp()
	{
		SetName(typeid(*this).name());
	}

	void PickUp::OnStart()
	{
		// create event handler
		m_collisionHandler = AIngine::Events::EventHandler<void, PhysicsComponent*>(std::bind(&PickUp::OnCollision, this, std::placeholders::_1));
		// register callback
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin += m_collisionHandler;

		m_player = AIngine::World::GetGameObject("PlayerRocket")->GetComponent<Player>();
	}

	void PickUp::OnEnd()
	{
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin -= this->m_collisionHandler;
		GetOwner()->SetActive(false);
	}

	void PickUp::Update(float deltatime)
	{
		GetOwner()->Translate(glm::vec2(-CrappyBird::s_GameSpeed  *deltatime, 0), true);

		// the pickUp is now beyond the player
		if (GetOwner()->GetLocalPosition().x <= -pickUpRadius)
		{
			GetOwner()->SetActive(false);
		}
	}

	void PickUp::OnCollision(AIngine::Physics::PhysicsComponent * other)
	{
		if (other->GetOwner()->GetName() == "PlayerRocket") {
			m_player->AddEffect(m_Effect);
			GetOwner()->SetActive(false);
		}
	}
}