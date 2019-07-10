#include "Player.h"
#include "Pong.h"
#include "Rendering/Camera.h"

namespace Pong {

	// Constructor
	Player::Player()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	static glm::vec2 lastMousePos;

	// Start is called when gameplay starts for this script
	void Player::OnStart()
	{
		m_rigidBody = GetOwner()->GetComponent<PhysicsComponent>();
		m_BallBody = AIngine::World::GetGameObject("Ball")->GetComponent<PhysicsComponent>();
		lastMousePos = AIngine::Rendering::Camera::Get().WorldToScreenPoint(AIngine::World::GetCenter());

		m_rigidBody->OnCollisionBegin += std::bind(&Player::OnBallCollision, this, std::placeholders::_1);
	}

	// End is called when gameplay ends for this script
	void Player::OnEnd()
	{
		m_rigidBody->OnCollisionBegin = AIngine::Physics::CollisionEvent();
	}

	// Update is called once per frame
	void Player::Update(float delta)
	{
		if (m_HasBall) {
			m_BallBody->GetOwner()->SetWorldPosition(GetOwner()->GetWorldPosition() + m_ballOffset);
		}
	}

	// Callback for events
	void Player::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void Player::ReceiveBall()
	{
		glm::vec playerSize = GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize();
		int direction = Role == PlayerRole::One ? 1 : -1;
		float xPos = GetOwner()->GetWorldPosition().x + direction * playerSize.x * 0.6 + direction * m_BallBody->GetBodyInformation().radius;
		float minHeight = GetOwner()->GetWorldPosition().y - playerSize.y * 0.5f + +m_BallBody->GetBodyInformation().radius;
		float maxHeight = GetOwner()->GetWorldPosition().y + playerSize.y * 0.5f - +m_BallBody->GetBodyInformation().radius;
		float height = AIngine::Util::Random::RandomFloat(minHeight, maxHeight);
		m_ballOffset = glm::vec2(xPos, height) - GetOwner()->GetWorldPosition();
		m_HasBall = true;
	}

	void Player::StartBall()
	{
		m_HasBall = false;
		float forceY =/* AIngine::Util::Random::RandomFloat(-8, 8)*/ 10;
		if (forceY == 0) forceY = 1;
		m_BallBody->ApplyForceToCenter(glm::vec2(25, forceY));
	}

	void Player::MovePlayerWithMouse()
	{
		static float minY = AIngine::World::GetBounds().z + GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;
		static float maxY = AIngine::World::GetBounds().w - GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;

		glm::vec2 currentMousePos = glm::vec2(Input::GetMouseX(), Input::GetMouseY());
		float delta = (currentMousePos.y - lastMousePos.y);
		if (delta < 0) delta = -TranslationRate * Pong::Get().GetDeltaTime();
		else delta = TranslationRate * Pong::Get().GetDeltaTime();
		float currentHeight = GetOwner()->GetWorldPosition().y;

		if (currentHeight + delta > maxY) {
			delta = maxY - currentHeight;
		}
		else if (currentHeight + delta < minY) {
			delta = minY - currentHeight;
		}

		m_rigidBody->GetOwner()->Translate(glm::vec2(0, delta));
		lastMousePos = currentMousePos;

	}

	void Player::MovePlayerWithKeyboard(AIngine::Events::KeyPressedEvent::KeyEventData & e, AIngine::KeyCode up, AIngine::KeyCode down)
	{
		static float minY = AIngine::World::GetBounds().z + GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;
		static float maxY = AIngine::World::GetBounds().w - GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;

		float delta = 0;
		float currentHeight = GetOwner()->GetWorldPosition().y;

		if (e.GetKeyCode() == up) {
			delta = TranslationRate * Pong::Get().GetDeltaTime();
		}

		else if (e.GetKeyCode() == down) {
			delta = -TranslationRate * Pong::Get().GetDeltaTime();
		}

		if (currentHeight + delta > maxY) {
			delta = maxY - currentHeight;
		}
		else if (currentHeight + delta < minY) {
			delta = minY - currentHeight;
		}

		m_rigidBody->GetOwner()->Translate(glm::vec2(0, delta));
	}

}