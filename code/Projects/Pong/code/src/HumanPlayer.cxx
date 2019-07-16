#include "HumanPlayer.h"
#include "Pong.h"
#include "Rendering/Camera.h"
namespace Pong {

	// Constructor
	HumanPlayer::HumanPlayer()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void HumanPlayer::OnStart()
	{
		Player::OnStart();

		glm::vec2 screenPos = AIngine::Rendering::Camera::Get().WorldToScreenPoint(GetOwner()->GetWorldPosition());

		AIngine::Input::SetMousePos({ screenPos.x, screenPos.y });
	}

	// End is called when gameplay ends for this script
	void HumanPlayer::OnEnd()
	{
		Player::OnEnd();
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
	}
}