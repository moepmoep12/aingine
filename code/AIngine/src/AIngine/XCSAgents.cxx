#include "AIngine/XCSAgents.h"

namespace AIngine {

	XCSAgentSupervisor::XCSAgentSupervisor()
	{
		m_xcsr = std::make_unique<xxr::xcsr_impl::Experiment<double, int>>(PossibleActions, xxr::XCSRConstants(), Representation);
	}

	void XCSAgentSupervisor::ProvideReward(Agent * agent, bool isEnd)
	{
		m_xcsr->reward(agent->GetStepReward(), isEnd);
	}

	int XCSAgentSupervisor::ChooseAction(const std::vector<double>& situation)
	{
		return Exploration ? m_xcsr->explore(situation) : m_xcsr->exploit(situation, true);
	}

	void XCSAgentSupervisor::ChangeRepresentation(xxr::xcsr_impl::Repr repr)
	{
		if (repr == Representation) return;

		m_xcsr = std::make_unique<xxr::xcsr_impl::Experiment<double, int>>(PossibleActions, GetConstants(), repr);
		Representation = repr;
	}

	xxr::XCSRConstants & XCSAgentSupervisor::GetConstants()
	{
		//if (!m_xcsr) return Constants;
		xxr::xcsr_impl::csr::Experiment<double, int>* xcsrCS;
		xxr::xcsr_impl::obr::Experiment<double, int>* xcsrOBR;
		xxr::xcsr_impl::ubr::Experiment<double, int>* xcsrUBR;
		switch (Representation) {
		case xxr::xcsr_impl::Repr::CSR:
			xcsrCS = dynamic_cast<xxr::xcsr_impl::csr::Experiment<double, int>*>(&m_xcsr->GetExperiment());
			if (xcsrCS)
				return xcsrCS->GetConstants();
			break;

		case xxr::xcsr_impl::Repr::OBR:
			xcsrOBR = dynamic_cast<xxr::xcsr_impl::obr::Experiment<double, int>*>(&m_xcsr->GetExperiment());
			if (xcsrOBR)
				return xcsrOBR->GetConstants();
			break;


		case xxr::xcsr_impl::Repr::UBR:
			xcsrUBR = dynamic_cast<xxr::xcsr_impl::ubr::Experiment<double, int>*>(&m_xcsr->GetExperiment());
			if (xcsrUBR)
				return xcsrUBR->GetConstants();
			break;
		}
	}
}