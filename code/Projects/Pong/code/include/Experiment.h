#pragma once
#include "AIngine/Core.h"
#include "Player.h"

#include "UI/Text.h"

namespace Pong {
	class Experiment : public AIngine::Script {
	public:
		Experiment();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;
		virtual void OnGUI() override;

		PhysicsComponent* LeftEdge;
		PhysicsComponent* RightEdge;
		PhysicsComponent* TopEdge;
		PhysicsComponent* BottomEdge;
		Player* PlayerOne;
		Player* PlayerTwo;
		PhysicsComponent* BallBody;

		int ScorePlayerOne;
		int ScorePlayerTwo;

		AIngine::Structures::RectangleF ArenaRect;

	private:
		AIngine::UI::UIText* ScoreText;

	private:
		void OnCollisionLeft(AIngine::Physics::Contact contact);
		void OnCollisionRight(AIngine::Physics::Contact contact);
	};
}