#include "Player.h"
#include "AIngine/Input.h"
#include "Events/Event.h"
#include "Events/InputEvents.h"
#include "CrappyBird.h"

#include <typeinfo>

namespace CrappyBird {
	Player::Player()
	{
		SetName(typeid(*this).name());
		m_originalGameSpeed = CrappyBird::s_GameSpeed;
	}

	Player::~Player()
	{
	}

	glm::vec2 Player::GetSize()
	{
		return glm::vec2(1.4, 0.7);
	}

	void Player::OnStart()
	{
		GetOwner()->GetComponent<PhysicsComponent>()->SetFixedRotation(true);
		m_spawnPos = GetOwner()->GetLocalPosition();
		m_physBody = m_owner->GetComponent<AIngine::Physics::PhysicsComponent>();
		m_collisionHandler = AIngine::Events::EventHandler<void, PhysicsComponent*>(std::bind(&Player::OnCollision, this, std::placeholders::_1));
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin += m_collisionHandler;
	}

	void Player::OnEnd()
	{
		CrappyBird::s_GameSpeed = m_originalGameSpeed;
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin -= m_collisionHandler;
	}

	void Player::OnEvent(AIngine::Events::EventData & e)
	{
		// key pressed
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {
			AIngine::Events::KeyPressedEvent::KeyPressedEventData pressedEvent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);
			if (pressedEvent.GetKeyCode() == AIngine::KeyCodes::R) {
			}
		}
	}

	void Player::OnGUI()
	{
		std::stringstream ss;
		ss << "Score: " << (int)m_distanceTraveled;
		Graphics::Text(ss.str(), glm::vec2(10, 20), glm::vec2(2));
	}

	void Player::Update(float deltatime)
	{
		if (AIngine::Input::IsMouseButtonPressed(0)) {
			m_physBody->ApplyLinearImpulseToCenter(glm::vec2(0, -0.075f));
		}

		m_distanceTraveled += CrappyBird::s_GameSpeed * deltatime;

		if (m_distanceTraveled >= CurrentScreenIndex * 10) {
			CurrentScreenIndex++;
			glm::vec4 bounds = AIngine::World::GetBounds();
			AIngine::Structures::RectangleI nextScreen(bounds.y, 0, bounds.y, bounds.w / 2.0f);
			OnEnterNewScreen(nextScreen);
		}
	}


	void Player::OnCollision(PhysicsComponent * other)
	{
		if (other->GetOwner()->GetName() == "Obstacle") {
			GetOwner()->SetActive(false);
		}
	}
}