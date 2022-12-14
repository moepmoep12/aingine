#pragma once
#include "AIngine/Core.h"
#include "Pong.h"


namespace Pong {
	class Experiment;

	class Player : public AIngine::Script {
	public:
		Player();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		virtual void ReceiveBall();

		virtual void OnScored(PlayerRole goalie) {}
		virtual void OnBallCollision(AIngine::Physics::Contact contact) {}

	public:
		float TranslationRate = 6;
		PlayerRole Role;

	protected:
		virtual void StartBall();
		//virtual void MovePlayerWithMouse();
		//virtual void MovePlayerWithKeyboard(AIngine::Events::KeyPressedEvent::KeyEventData& e, AIngine::KeyCode up, AIngine::KeyCode down);
		void Move(int direction);


	protected:
		Experiment* m_experiment = nullptr;
		PhysicsComponent* m_rigidBody;
		glm::vec2 m_ballOffset;
		bool m_HasBall = false;
		float minY;
		float maxY;
	};
}