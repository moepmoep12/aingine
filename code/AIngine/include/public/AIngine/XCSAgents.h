#pragma once
#include "AIngine/Agent.h"

#include <xxr/xcsr.hpp>
#include <memory>

namespace AIngine {
	class XCSAgentSupervisor : public AgentSupervisor {
		// Inherited via AgentSupervisor
		virtual void ProvideReward(Agent * agent, bool isEnd) override;
		virtual int ChooseAction(const std::vector<double>& situation) override;

	public:
		XCSAgentSupervisor();
		void ChangeRepresentation(xxr::xcsr_impl::Repr repr);

		xxr::xcsr_impl::Repr GetRepresentation() const { return Representation; }

		xxr::XCSRConstants& GetConstants();

		std::string PopulationPath;

	public:
		std::unique_ptr<xxr::xcsr_impl::Experiment<double, int>> m_xcsr;
		std::unordered_set<int> PossibleActions = { 0,1 };
		bool Exploration = true;

	private:
		xxr::xcsr_impl::Repr Representation = xxr::xcsr_impl::Repr::CSR;
		//xxr::XCSRConstants Constants;
	};
}