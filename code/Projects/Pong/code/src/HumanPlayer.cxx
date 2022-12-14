#include "HumanPlayer.h"
#include "Pong.h"
#include "Rendering/Camera.h"
namespace Pong {

	// Constructor
	HumanPlayer::HumanPlayer()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
		Role = PlayerRole::One;
	}

	// Start is called when gameplay starts for this script
	void HumanPlayer::OnStart()
	{
		glm::vec2 size = GetComponent<Sprite>()->GetLocalWorldSize();
		size.y *= Pong::PlayerSizes[Pong::Difficulty];
		GetComponent<Sprite>()->SetLocalWorldSize(size);

		float height = GetComponent<PhysicsComponent>()->GetBodyInformation().height;
		float width = GetComponent<PhysicsComponent>()->GetBodyInformation().width;
		height *= Pong::PlayerSizes[Pong::Difficulty];
		GetComponent<PhysicsComponent>()->AdjustBoxShape(width, height);

		Player::OnStart();

		glm::vec2 screenPos = AIngine::Rendering::Camera::Get().WorldToScreenPoint(GetOwner()->GetWorldPosition());

		AIngine::Input::SetMousePos({ screenPos.x, screenPos.y });

		AIngine::World::GetGameObject("BackGround")->GetComponent<SoundComponent>()->Play(0);

		#ifndef EDITOR
		Pong::Get().GetWindow().SetMouseVisible(false);
		#endif

	}

	// End is called when gameplay ends for this script
	void HumanPlayer::OnEnd()
	{
		Player::OnEnd();
		Pong::Get().GetWindow().SetMouseVisible(true);
#ifndef EDITOR
		AIngine::World::GetGameObject("BackGround")->GetComponent<SoundComponent>()->Stop(0);
#endif
	}

	// Update is called once per frame
	void HumanPlayer::Update(float delta)
	{
		Player::Update(delta);


	}

	// Callback for events
	void HumanPlayer::OnEventData(AIngine::Events::EventData & e)
	{
		if (!Pong::IsRunning()) return;

		if (typeid(e) == typeid(AIngine::Events::MouseButtonReleasedEvent::MouseButtonReleasedEventData)) {
			AIngine::Events::MouseButtonReleasedEvent::MouseButtonReleasedEventData pressedEvent = dynamic_cast<AIngine::Events::MouseButtonReleasedEvent::MouseButtonReleasedEventData&>(e);
			if (pressedEvent.GetMouseButton() == AIngine::MouseButton::BUTTON_LEFT) {
				if (m_HasBall) StartBall();
			}
		}

		if (typeid(e) == typeid(AIngine::Events::MouseMovedEvent::MouseMovedEventData)) {
			glm::vec2 currentMousePos = glm::vec2(Input::GetMouseX(), Input::GetMouseY());
			glm::vec2 resolution = Pong::Get().GetWindow().GetMonitorResolution();
			currentMousePos.y = std::clamp(currentMousePos.y, 0.0f, resolution.y);
			glm::vec2 mouseWorldPos = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(currentMousePos);
			mouseWorldPos.y = std::clamp(mouseWorldPos.y, 0.0f, 10.0f * (9.0f / 16.0f));
			glm::vec2 currentPos = GetOwner()->GetWorldPosition();
			float delta = (mouseWorldPos.y - currentPos.y);
			float currentHeight = GetOwner()->GetWorldPosition().y;

			if (currentHeight + delta > maxY) {
				delta = maxY - currentHeight;
			}
			else if (currentHeight + delta < minY) {
				delta = minY - currentHeight;
			}

			m_rigidBody->GetOwner()->SetWorldPosition(glm::vec2(currentPos.x, std::clamp(currentPos.y + delta, minY, maxY)));
		}

		// key pressed
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {
			AIngine::Events::KeyPressedEvent::KeyPressedEventData pressedEvent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);

			if (pressedEvent.GetKeyCode() == AIngine::KeyCode::ESCAPE) {
				Pong::LoadScene(0);
			}
		}
	}
}