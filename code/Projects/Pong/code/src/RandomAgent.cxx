#include "RandomAgent.h"
namespace Pong {

	// Constructor
	RandomAgent::RandomAgent()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void RandomAgent::OnStart()
	{
		Player::OnStart();
		//ReceiveBall();
	}

	// End is called when gameplay ends for this script
	void RandomAgent::OnEnd()
	{
		Player::OnEnd();
	}

	// Update is called once per frame
	void RandomAgent::Update(float delta)
	{
		Player::Update(delta);

		if (receivedBall) {
			receivedBall = false;

			static float minY = AIngine::World::GetBounds().z + GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;
			static float maxY = AIngine::World::GetBounds().w - GetOwner()->GetComponent<Sprite>()->GetLocalWorldSize().y * 0.5;

			float randomHeight = AIngine::Util::Random::RandomFloat(minY, maxY);
			glm::vec2 pos = GetOwner()->GetWorldPosition();
			pos.y = randomHeight;
			m_rigidBody->GetOwner()->SetWorldPosition(pos);
		}

		if (m_HasBall)
			Player::StartBall();

		Move(AIngine::Util::Random::RandomInt(-1, 1));
	}

	// Callback for events
	void RandomAgent::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void RandomAgent::ReceiveBall()
	{
		Player::ReceiveBall();
		receivedBall = true;


	}
}