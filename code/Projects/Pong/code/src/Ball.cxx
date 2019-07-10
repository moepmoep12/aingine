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
		for (auto comp : comps)
			if (dynamic_cast<Player*>(comp)) {
				PlayerOne = dynamic_cast<Player*>(comp);
				break;
			}

		PlayerOne->Role = PlayerRole::One;
		PlayerOne->ReceiveBall();

		comps = AIngine::World::GetGameObject("PlayerTwo")->GetComponents();
		for (auto comp : comps)
			if (dynamic_cast<Player*>(comp)) {
				PlayerTwo = dynamic_cast<Player*>(comp);
				break;
			}
		PlayerTwo->Role = PlayerRole::Two;

		LeftEdge->OnCollisionBegin += std::bind(&Ball::OnCollisionLeft, this, std::placeholders::_1);
		RightEdge->OnCollisionBegin += std::bind(&Ball::OnCollisionRight, this, std::placeholders::_1);

		ScoreText = AIngine::World::GetGameObject("ScoreText")->GetComponent<AIngine::UI::UIText>();

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

	void Ball::OnGUI()
	{
		std::stringstream ss;
		ss << Pong::ScorePlayerOne << "  :  " << Pong::ScorePlayerTwo;
		ScoreText->Text = ss.str();

		static AIngine::Structures::RectangleF worldRect = AIngine::World::GetWorldRect();
		AIngine::Graphics::BoxWorld(worldRect, glm::vec4(0, 0, 0, 1));
	}

	void Ball::OnCollisionLeft(AIngine::Physics::Contact contact)
	{
		Pong::ScorePlayerTwo++;
		PlayerTwo->OnScored(PlayerRole::Two);
		PlayerOne->OnScored(PlayerRole::Two);
		PlayerTwo->ReceiveBall();
	}

	void Ball::OnCollisionRight(AIngine::Physics::Contact contact)
	{
		Pong::ScorePlayerOne++;
		PlayerTwo->OnScored(PlayerRole::One);
		PlayerOne->OnScored(PlayerRole::One);
		PlayerOne->ReceiveBall();
	}
}