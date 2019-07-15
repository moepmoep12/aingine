#include "AIngine/Agent.h"
#include "Application.h"

namespace AIngine {

	void AgentSupervisor::OnUpdate(float deltatime)
	{
		if (!AIngine::Application::IsRunning()) return;

		m_stepCount++;
		if (m_stepCount >= StepRate) {
			m_stepCount = 0;
			for (auto& agent : m_agents) {
				bool isEnd = false;
				if (agent->IsEndOfProblem())
				{
					agent->StepCount = 0;
					agent->Reset();
					isEnd = true;
				}
				else if (MaxSteps > 0 && m_stepCount > MaxSteps)
				{
					agent->StepCount = 0;
					agent->OnMaxStepsReached();
					agent->Reset();
					isEnd = true;
				}
				else {
					agent->StepCount++;
					const std::vector<double>& observation = agent->Observe();

					assert(observation.size() == ObservationsSize);

					int action = ChooseAction(observation);
					agent->ExecuteAction(action);
				}
				ProvideReward(agent, isEnd);
				agent->FinishStep();
			}
		}
	}

	void AgentSupervisor::AddAgent(Agent * agent)
	{
		if (std::find(m_agents.begin(), m_agents.end(), agent) == m_agents.end()) {
			m_agents.push_back(agent);
		}
	}
}