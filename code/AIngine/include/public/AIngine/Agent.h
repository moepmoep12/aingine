#pragma once
#include "Component.h"
#include "Script.h"

#include <vector>
#include <string>
//#include <memory>
//#include <xxr/xcs.hpp>
//#include <xxr/xcsr.hpp>

namespace AIngine {

	class Agent;

	class AgentSupervisor : public Component {
	public:
		/* The number of observations an agent will make at every step. */
		int ObservationsSize = 0;

		/* Request a decision every X steps. Where 0 means on every tick */
		int StepRate = 0;

		/* The amount of steps an agent is allowed to make at maximum.
		*  Where 0 = unlimited */
		int MaxSteps = 0;

		// Inherited via Component
		virtual void OnUpdate(float deltatime) override;

		virtual void AddAgent(Agent* agent);
		virtual void RemoveAgent(int index);
		virtual void RemoveAgent(Agent* agent);

		virtual void ProvideReward(Agent* agent, bool isEnd) = 0;
		virtual int ChooseAction(const std::vector<double>& situation) = 0;

		const std::vector<Agent*>& GetAgents() const { return m_agents; }

	protected:
		std::vector<Agent*> m_agents;
		int m_currentStep = 0;


	};

	class Agent {

	public:
		/* Returns the reward accumulated in this episdode so far */
		float GetCumulativeReward() const { return m_cumulativeReward; }

		float GetStepReward() const { return m_stepReward; }

		/* Returns a vector of observations with the same size as the ObservationCount of the AgentSupervisor */
		virtual std::vector<double> Observe() = 0;

		/* Returns whether the agent reached the end of the problem either via success or failing */
		virtual bool IsEndOfProblem() = 0;

		/* Executes the given action */
		virtual void ExecuteAction(int action) = 0;


		virtual void OnMaxStepsReached() = 0;

		virtual std::string Name() const = 0;

		void AddReward(float reward) {
			m_stepReward += reward;
			m_cumulativeReward += reward;
		}

		virtual float Performance() { return 0; }

		void FinishStep() { m_stepReward = 0; }

		void FinishEpisode() { m_cumulativeReward = 0; StepCount = 0; m_stepReward = 0; Reset(); }

		/* The number of steps taken in this episode */
		int StepCount = 0;

		int ID = -1;

		std::string SupervisorName;

		/* Resets this agent.
		*  Occurs when the problem was either solved, failed or the maxStepCount was exceeded */
	protected:
		virtual void Reset() = 0;

	private:
		float m_cumulativeReward = 0;
		float m_stepReward = 0;

	};
}