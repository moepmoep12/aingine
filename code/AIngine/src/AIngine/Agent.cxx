#include "AIngine/Agent.h"
#include "Application.h"

namespace AIngine {

	void AgentSupervisor::OnUpdate(float deltatime)
	{
		if (!AIngine::Application::IsRunning()) return;

		m_currentStep++;
		if (m_currentStep >= StepRate) {
			m_currentStep = 0;
			for (auto& agent : m_agents) {
				bool isEnd = false;
				if (MaxSteps > 0 && m_currentStep > MaxSteps)
				{
					agent->OnMaxStepsReached();
					isEnd = true;
				}
				else {
					agent->StepCount++;
					const std::vector<double>& observation = agent->Observe();
					if (ObservationsSize == 0) ObservationsSize = observation.size();
					assert(observation.size() == ObservationsSize);

					int action = ChooseAction(observation);
					agent->ExecuteAction(action);
				}

				if (agent->IsEndOfProblem()) isEnd = true;

				ProvideReward(agent, isEnd);

				if (isEnd)
					agent->FinishEpisode();
				else
					agent->FinishStep();
			}
		}
	}

	void AgentSupervisor::AddAgent(Agent * agent)
	{
		if (std::find(m_agents.begin(), m_agents.end(), agent) == m_agents.end()) {
			agent->ID = m_agents.size();
			agent->SupervisorName = GetOwner()->GetName();
			m_agents.push_back(agent);
		}
	}

	void AgentSupervisor::RemoveAgent(int index)
	{
		if (index < m_agents.size()) {
			auto result = std::find(m_agents.begin(), m_agents.end(), m_agents.at(index));
			if (result != m_agents.end()) {
				auto erased = m_agents.erase(result);
				(*erased._Ptr)->SupervisorName = "";
				(*erased._Ptr)->ID = -1;
			}
		}
	}

	void AgentSupervisor::RemoveAgent(Agent * agent)
	{
		auto result = std::find(m_agents.begin(), m_agents.end(), agent);
		if (result != m_agents.end()) {
			m_agents.erase(result);
			(*result._Ptr)->SupervisorName = "";
			(*result._Ptr)->ID = -1;
		}
	}
}