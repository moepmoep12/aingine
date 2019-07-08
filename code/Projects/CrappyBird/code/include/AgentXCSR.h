#pragma once
#include "AIngine/Core.h"
#include "Player.h"

#include "xxr/environment/environment.hpp"
#include "xxr/xcsr.hpp"

namespace CrappyBird {
	class AgentXCSR : public Player {
	public:
		AgentXCSR();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;
		virtual void PostInit() override;

		virtual void ResetGame() override;
		virtual void OnGameOver() override;
		virtual void OnCollision(AIngine::Physics::Contact contact) override;

		// Inherited via AbstractEnvironment
		virtual std::vector<double> situation();
		virtual double executeAction(bool action);
		virtual bool isEndOfProblem() const;

	public:
		//xxr::XCSR<double, bool> m_xcsr;
		xxr::XCSR<>* m_xcsr;
	};
}