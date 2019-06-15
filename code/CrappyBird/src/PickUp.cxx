#include "PickUp.h"
#include "CrappyBird.h"
#include "Player.h"

#include <random>
#include <time.h>

namespace CrappyBird {

	static const float pickUpRadius = 0.2f;

	void PickUpFactory::OnStart()
	{
		srand(time(NULL));
		AIngine::World::GetGameObject("PlayerRocket")->GetComponent<Player>()->OnEnterNewScreen += [=]() {
			glm::vec4 bounds = AIngine::World::GetBounds();
			AIngine::Structures::RectangleI nextScreen(bounds.y, 0, bounds.y, bounds.w / 2.0f);
			this->SpawnPickUpInArea(nextScreen);
		};
	}

	void PickUpFactory::SpawnPickUpInArea(const AIngine::Structures::RectangleI & worldRect)
	{
		GameObject* availableObj = GetAvailableGameObject();
		if (availableObj) {
			glm::vec2 min = worldRect.GetPosition() + glm::vec2(pickUpRadius);
			glm::vec2 bottomRight = worldRect.GetMax() + glm::vec2(-pickUpRadius, -pickUpRadius);

			float x = min.x + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / bottomRight.x);
			float y = min.y + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / bottomRight.y);
			glm::vec2 spawnPos = glm::vec2(x, y);

			availableObj->SetLocalPosition(spawnPos);
			availableObj->AddComponent<SpeedPickUp>();
			availableObj->SetActive(true);
		}
		else {

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
	/******************************** PICKUP ******************************************/


	void PickUp::OnStart()
	{
		m_collisionHandler = AIngine::Events::EventHandler<void, PhysicsComponent*>(std::bind(&PickUp::OnCollision, this, std::placeholders::_1));
		GetOwner()->SetActive(true);
		GetOwner()->GetComponent<Sprite>()->SetEnabled(true);
		GetOwner()->GetComponent<PhysicsComponent>()->SetEnabled(true);
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin += m_collisionHandler;
	}

	void PickUp::Update(float deltatime)
	{
		GetOwner()->Translate(glm::vec2(-CrappyBird::s_GameSpeed  *deltatime, 0), true);
		if (GetOwner()->GetLocalPosition().x <= -pickUpRadius) {
			OnFinish();
		}
	}

	void PickUp::OnFinish()
	{
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin -= this->m_collisionHandler;
		GetOwner()->SetActive(false);
		Destroy();
	}

	void PickUp::OnCollision(AIngine::Physics::PhysicsComponent * other)
	{
		if (other->GetOwner()->GetName() == "PlayerRocket") {
			GetOwner()->GetComponent<PhysicsComponent>()->SetEnabled(false);
			GetOwner()->GetComponent<Sprite>()->SetEnabled(false);
			this->OnPickUp();
		}
	}

	/******************************** SPEED PICKUP ******************************************/

	void SpeedPickUp::OnPickUp()
	{
		Active = true;
		CrappyBird::s_GameSpeed += 2.0f;
		DEBUG_INFO("Increased speed to {0}", CrappyBird::s_GameSpeed);
	}

	void SpeedPickUp::Update(float deltatime)
	{
		if (Active) {
			Duration -= deltatime;
			if (Duration <= 0) {
				CrappyBird::s_GameSpeed -= 2.0f;
				DEBUG_INFO("Reduced speed to {0}", CrappyBird::s_GameSpeed);
				OnFinish();
			}
		}
		else
			PickUp::Update(deltatime);
	}

	/******************************** SLOWSPEED PICKUP ******************************************/

	void SlowSpeedPickUp::OnPickUp()
	{
		Active = true;
		CrappyBird::s_GameSpeed -= 1.5f;
		GetOwner()->GetComponent<Sprite>()->SetEnabled(false);
	}

	void SlowSpeedPickUp::Update(float deltatime)
	{
		if (Active) {
			Duration -= deltatime;
			if (Duration <= 0) {
				CrappyBird::s_GameSpeed += 1.5f;
				OnFinish();
			}
		}
		else
			PickUp::Update(deltatime);
	}
}