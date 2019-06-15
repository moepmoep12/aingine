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

	void Player::OnStart()
	{
		m_spawnPos = GetOwner()->GetLocalPosition();
		m_physBody = m_owner->GetComponent<AIngine::Physics::PhysicsComponent>();
		m_pickUpFactory = AIngine::World::GetGameObject("PickUpFactory")->GetComponent<PickUpFactory>();
		glm::vec4 bounds = AIngine::World::GetBounds();
		AIngine::Structures::RectangleI nextScreen(bounds.y, 0, bounds.y, bounds.w / 2.0f);
		m_pickUpFactory->SpawnPickUpInArea(nextScreen);

		OnEnterNewScreen += [=]() {
			DEBUG_INFO(this->m_distanceTraveled);
		};
	}

	void Player::OnEnd()
	{
		CrappyBird::s_GameSpeed = m_originalGameSpeed;
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
			OnEnterNewScreen();
		}
	}

	Player::~Player()
	{
	}
}