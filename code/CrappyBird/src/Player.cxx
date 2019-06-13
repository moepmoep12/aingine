#include "Player.h"
#include "AIngine/Input.h"
#include "Events/Event.h"
#include "Events/InputEvents.h"

#include <typeinfo>

namespace CrappyBird {
	Player::Player(AIngine::GameObject * owner)
		: Script(owner)
	{
		SetName(typeid(*this).name());
	}

	void Player::OnStart()
	{
		m_physBody = m_owner->GetComponent<AIngine::Physics::PhysicsComponent>();
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

	void Player::Update(float deltatime)
	{
		if (AIngine::Input::IsMouseButtonPressed(0)) {
			m_physBody->ApplyLinearImpulseToCenter(glm::vec2(0, -0.075f));
		}
	}

	CrappyBird::Player::~Player()
	{
	}
}