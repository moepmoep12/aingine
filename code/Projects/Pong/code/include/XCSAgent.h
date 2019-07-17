#pragma once
#include "AIngine/Core.h"
#include "AIngine/Agent.h"
#include "Player.h"

namespace Pong {
	class XCSAgent : public AIngine::Agent, public Player {
	public:
		XCSAgent();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnGUI() override;
		virtual void OnWidget() override;

		// Inherited via Player
		virtual void OnScored(PlayerRole goalie) override;

		// Inherited via Agent
		virtual std::vector<double, std::allocator<double>> Observe();
		virtual bool IsEndOfProblem();
		virtual void ExecuteAction(int action);
		virtual void Reset();
		virtual void OnMaxStepsReached();
		virtual std::string Name() const { return Script::GetName(); }


	private:
		std::vector<glm::vec2> CalcIntersections();
		bool DoesBallCollide();

	private:
		int lastAction = 0;
		double collisionPointY = 0;
		double collisionPointX = 0;
		double distanceToCollisionPoint = 0;
		bool scored = false;
		PlayerRole goalie;
		std::vector<std::pair<int, int>> scores;
		bool testing = false;
		std::vector<double> observation;

	};
}