#include "Ball.h"
#include "Pong.h"

namespace Pong {

	// Constructor
	Ball::Ball()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void Ball::OnStart()
	{
		LeftEdge = AIngine::World::GetGameObject("LeftEdge")->GetComponent<PhysicsComponent>();
		RightEdge = AIngine::World::GetGameObject("RightEdge")->GetComponent<PhysicsComponent>();
		auto comps = AIngine::World::GetGameObject("PlayerOne")->GetComponents();
		for (auto& comp : comps)
			if (static_cast<Player*>(comp)) {
				PlayerOne = static_cast<Player*>(comp);
				break;
			}

		PlayerOne->Role = PlayerRole::One;

		 comps = AIngine::World::GetGameObject("PlayerTwo")->GetComponents();
		for (auto& comp : comps)
			if (static_cast<Player*>(comp)) {
				PlayerTwo = static_cast<Player*>(comp);
				break;
			}

		LeftEdge->OnCollisionBegin += std::bind(&Ball::OnCollisionLeft, this, std::placeholders::_1);
		RightEdge->OnCollisionBegin += std::bind(&Ball::OnCollisionRight, this, std::placeholders::_1);
	}

	// End is called when gameplay ends for this script
	void Ball::OnEnd()
	{
		LeftEdge->OnCollisionBegin = AIngine::Physics::CollisionEvent();
		RightEdge->OnCollisionBegin = AIngine::Physics::CollisionEvent();

		Pong::ScorePlayerOne = 0;
		Pong::ScorePlayerTwo = 0;
	}

	// Update is called once per frame
	void Ball::Update(float delta)
	{
	}

	// Callback for events
	void Ball::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void Ball::OnCollisionLeft(AIngine::Physics::Contact contact)
	{
		Pong::ScorePlayerTwo++;
		PlayerTwo->OnScored(PlayerRole::Two);
		PlayerOne->OnScored(PlayerRole::Two);
		PlayerOne->ReceiveBall();
	}

	void Ball::OnCollisionRight(AIngine::Physics::Contact contact)
	{
		Pong::ScorePlayerOne++;
		PlayerTwo->OnScored(PlayerRole::One);
		PlayerOne->OnScored(PlayerRole::One);
		PlayerTwo->ReceiveBall();
	}
}