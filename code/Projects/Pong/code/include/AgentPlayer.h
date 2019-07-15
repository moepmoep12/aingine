#pragma once
#include "AIngine/Core.h"
#include "Player.h"
#include "xxr/xcsr.hpp"
#include "Ball.h"

namespace Pong {
	class AgentPlayer : public Player {
	public:
		AgentPlayer();
		virtual ~AgentPlayer();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;
		virtual void OnGUI() override;
		virtual void OnWidget() override;

		virtual void OnScored(PlayerRole role) override;

		void CreateConstantsWidget(xxr::XCSRConstants& constants);

	private:
		virtual std::vector<double> situation();
		void EndExperiment(float reward);
		double ComputeBallCollisionPoint();
		void ShowPopulation(bool& show);

	private:
		xxr::XCSR<>* m_xcsr;
		Ball* m_ball;
		PlayerRole goalie;
		bool scored = false;
		bool explore = true;
		int currentTick = 0;
		int guiTick = 0;
		int lastActionTaken = 0;
		const int guiTicks = 60;
		bool giveEnemyBall = false;
		xxr::XCSRConstants Constants;
	};
}