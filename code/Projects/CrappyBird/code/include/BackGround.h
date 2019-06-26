#pragma once
#include "AIngine/Core.h"
#include "Player.h"

namespace CrappyBird {
	class BackGround : public AIngine::Script {
	public:
		BackGround();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

	private:
		Player* m_player;

		Player::OnGameOverEventHandler OnGameOverHandler;
		Player::OnRestartGameEventHandler OnRestartGameHandler;

		void OnGameOver();
		void OnRestartGame();
	};
}