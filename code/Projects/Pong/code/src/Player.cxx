#include "Player.h"
#include "Pong.h"
#include "Rendering/Camera.h"
#include "GameManager.h"
#include "Experiment.h"
#include "AIngine/Constants.h"

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
namespace Pong {

	// Constructor
	Player::Player()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void Player::OnStart()
	{
		m_rigidBody = GetOwner()->GetComponent<PhysicsComponent>();

		m_rigidBody->OnCollisionBegin += std::bind(&Player::OnBallCollision, this, std::placeholders::_1);

		m_experiment = GetOwner()->GetParent()->GetComponent<Experiment>();

		minY = m_experiment->ArenaRect.y + GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;
		maxY = m_experiment->ArenaRect.GetMax().y - GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;
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
			m_experiment->BallBody->GetOwner()->SetWorldPosition(GetOwner()->GetWorldPosition() + m_ballOffset);
		}
	}

	// Callback for events
	void Player::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void Player::ReceiveBall()
	{
		if (!m_experiment) m_experiment = GetOwner()->GetParent()->GetComponent<Experiment>();
		glm::vec playerSize = GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize();
		int direction = Role == PlayerRole::One ? 1 : -1;
		float xPos = GetOwner()->GetWorldPosition().x + direction * playerSize.x * 0.6 + direction * m_experiment->BallBody->GetBodyInformation().radius;
		float minHeight = GetOwner()->GetWorldPosition().y - playerSize.y * 0.5f + +m_experiment->BallBody->GetBodyInformation().radius;
		float maxHeight = GetOwner()->GetWorldPosition().y + playerSize.y * 0.5f - +m_experiment->BallBody->GetBodyInformation().radius;
		float height;
		if (minHeight >= maxHeight)
			height = minHeight;
		else
			height = AIngine::Util::Random::RandomFloat(minHeight, maxHeight);
		m_ballOffset = glm::vec2(xPos, height) - GetOwner()->GetWorldPosition();
		m_HasBall = true;
	}

	void Player::StartBall()
	{
		m_HasBall = false;
		glm::vec2 force;
		float angle = AIngine::Util::Random::RandomInt(45, 135) * AIngine::D2R;
		glm::mat4 mat(1);
		float direction = Role == PlayerRole::One ? 1 : -1;
		mat = glm::rotate(mat, angle, glm::vec3(0, 0, direction));
		glm::vec4 dir(0, 1, 0, 0);
		dir = dir * mat;
		force.x = dir.x;
		force.y = dir.y;
		force = glm::normalize(force);
		m_experiment->BallBody->ApplyLinearImpulseToCenter(force);
	}

	/*void Player::MovePlayerWithMouse()
	{
		static float minY = AIngine::World::GetBounds().z + GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;
		static float maxY = AIngine::World::GetBounds().w - GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;

		glm::vec2 currentMousePos = glm::vec2(Input::GetMouseX(), Input::GetMouseY());
		glm::vec2 mouseWorldPos = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(currentMousePos);
		glm::vec2 currentPos = GetOwner()->GetWorldPosition();

		float delta = (mouseWorldPos.y - currentPos.y);
		float currentHeight = GetOwner()->GetWorldPosition().y;

		if (currentHeight + delta > maxY) {
			delta = maxY - currentHeight;
		}
		else if (currentHeight + delta < minY) {
			delta = minY - currentHeight;
		}

		m_rigidBody->GetOwner()->SetWorldPosition(glm::vec2(currentPos.x, currentPos.y + delta));

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
	}*/

	void Player::Move(int direction)
	{

		float del = 0;
		float currentHeight = GetOwner()->GetWorldPosition().y;
		del = TranslationRate * direction * Pong::Get().GetDeltaTimeScaled();

		if (currentHeight + del > maxY) {
			del = maxY - currentHeight;
		}
		else if (currentHeight + del < minY) {
			del = minY - currentHeight;
		}

		m_rigidBody->GetOwner()->Translate(glm::vec2(0, del));
	}

}