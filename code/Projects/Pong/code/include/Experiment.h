#pragma once
#include "AIngine/Core.h"
#include "Player.h"
#include "AIngine/XCSAgents.h"
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

		SoundComponent* m_sound;
		PhysicsComponent* LeftEdge = nullptr;
		PhysicsComponent* RightEdge = nullptr;
		PhysicsComponent* TopEdge = nullptr;
		PhysicsComponent* BottomEdge = nullptr;
		Player* PlayerOne = nullptr;
		Player* PlayerTwo = nullptr;
		PhysicsComponent* BallBody = nullptr;

		int ScorePlayerOne = 0;
		int ScorePlayerTwo = 0;

		AIngine::Structures::RectangleF ArenaRect;

		bool m_training = true;

	private:
		AIngine::UI::UIText* ScoreText;
		AIngine::UI::UIText* TrainingText;
		int m_currentTrainingTicks = 0;
		AIngine::XCSAgentSupervisor* supervisor;
		std::unordered_map<int, int> m_TrainingTicksPerDifficulty;


	private:
		void OnCollisionLeft(AIngine::Physics::Contact contact);
		void OnCollisionRight(AIngine::Physics::Contact contact);
	};
}