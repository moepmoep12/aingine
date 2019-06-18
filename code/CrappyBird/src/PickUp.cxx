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
			AddPickUpScript(*availableObj);
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

	void PickUpFactory::AddPickUpScript(GameObject & obj)
	{
		//int value = rand() % 3;

		//switch (value) {
		//case 0:
		//	obj.AddComponent<SpeedPickUp>();
		//	break;

		//case 1:
		//	obj.AddComponent<SlowSpeedPickUp>();
		//	break;

		//case 2:
		//	if (AIngine::World::GetGameObject("PlayerRocket")->GetLocalScale().x == 1)
		//		obj.AddComponent<ShrinkPickUp>();
		//	break;
		//}
	}

	/******************************** PICKUP ******************************************/


	void PickUp::OnStart()
	{
		m_collisionHandler = AIngine::Events::EventHandler<void, PhysicsComponent*>(std::bind(&PickUp::OnCollision, this, std::placeholders::_1));
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin += m_collisionHandler;
		GetOwner()->SetActive(true);
		GetOwner()->GetComponent<Sprite>()->SetEnabled(true);
		GetOwner()->GetComponent<PhysicsComponent>()->SetEnabled(true);
		Start();
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

	void PickUp::ResetSprite()
	{
		GetOwner()->GetComponent<Sprite>()->SetLocalWorldSize(glm::vec2(2 * pickUpRadius));
		GetOwner()->GetComponent<Sprite>()->SetColor(glm::vec4(1));
		GetOwner()->SetRotation(0);
		GetOwner()->GetComponent<Sprite>()->SetTexture(AIngine::Rendering::Texture2D(Assets::Load<BitmapAsset>("assets/Intellgine/textures/Circle.png")->GetBitmap()));
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

	void SpeedPickUp::Start()
	{
		ResetSprite();
		GetOwner()->GetComponent<Sprite>()->SetTexture(AIngine::Rendering::Texture2D(Assets::Load<BitmapAsset>("assets/CrappyBird/textures/fast-forward.png")->GetBitmap()));
	}

	/******************************** SLOWSPEED PICKUP ******************************************/

	void SlowSpeedPickUp::OnPickUp()
	{
		Active = true;
		if (CrappyBird::s_GameSpeed > CrappyBird::s_originalGameSpeed - m_value) {
			CrappyBird::s_GameSpeed -= m_value;
			m_appliedReduction = m_value;
		}
		GetOwner()->GetComponent<Sprite>()->SetEnabled(false);
	}

	void SlowSpeedPickUp::Update(float deltatime)
	{
		if (Active) {
			Duration -= deltatime;
			if (Duration <= 0) {
				CrappyBird::s_GameSpeed += m_appliedReduction;
				OnFinish();
			}
		}
		else
			PickUp::Update(deltatime);
	}

	void SlowSpeedPickUp::Start()
	{
		ResetSprite();
		GetOwner()->GetComponent<Sprite>()->SetTexture(AIngine::Rendering::Texture2D(Assets::Load<BitmapAsset>("assets/CrappyBird/textures/fast-forward.png")->GetBitmap()));
		GetOwner()->GetComponent<Sprite>()->SetColor(glm::vec4(1, 0, 1,1));
		GetOwner()->SetRotation(M_PI);
	}

	/******************************** ShrinkPickUp PICKUP ******************************************/

	void ShrinkPickUp::OnPickUp()
	{
		Active = true;
		GetOwner()->GetComponent<Sprite>()->SetEnabled(false);
	}

	void ShrinkPickUp::Update(float deltatime)
	{
		if (Active)
		{
			m_currentLerpTime += deltatime * direction;

			if (direction == 1 && m_currentLerpTime > AnimDuration) {
				m_currentEffectDuration += deltatime;
				if (m_currentEffectDuration >= EffectDuration) {
					m_currentLerpTime = AnimDuration;
					direction = -1;
				}
				else return;
			}

			if (direction == -1 && m_currentLerpTime < 0) {
				AIngine::World::GetGameObject("PlayerRocket")->SetLocalScale(glm::vec2(1));
				OnFinish();
				return;
			}

			float t = 1.0f + AIngine::Math::SinErp(m_currentLerpTime / AnimDuration);

			AIngine::World::GetGameObject("PlayerRocket")->SetLocalScale(glm::vec2(t));
		}

		else {
			PickUp::Update(deltatime);
		}
	}

	void ShrinkPickUp::Start()
	{
		ResetSprite();
	}
}