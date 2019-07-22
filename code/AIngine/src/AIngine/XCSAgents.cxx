#include "AIngine/XCSAgents.h"

namespace AIngine {

	XCSAgentSupervisor::XCSAgentSupervisor()
	{
		m_xcsr = std::make_unique<xxr::xcsr_impl::Experiment<double, int>>(PossibleActions, xxr::XCSRConstants(), Representation);
		snapShots.reserve(MaxRecords);
		episodes.reserve(MaxRecords);
	}

	void XCSAgentSupervisor::ProvideReward(Agent * agent, bool isEnd)
	{
		m_xcsr->reward(agent->GetStepReward(), isEnd);

		// Take snapshot of finished episode
		if (isEnd) {
			if (episodes.size() < MaxRecords)
				episodes.push_back(EpisodeSnapshot{ (float)episodes.size(), (float)agent->StepCount, agent->GetCumulativeReward() });

			if (agent->GetCumulativeReward() > m_maxReward)
				m_maxReward = agent->GetCumulativeReward();
			if (agent->StepCount > m_maxDuration)
				m_maxDuration = agent->StepCount;
		}

		// Take snapshot of population
		if (m_recording) {
			m_recordTimer++;
			if (m_recordTimer >= m_recordInterval) {
				m_recordTimer = 0;
				GetPopulationSnapShot(true);
			}
		}
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

	XCSAgentSupervisor::PopulationSnapshot XCSAgentSupervisor::GetPopulationSnapShot(bool record)
	{
		PopulationSnapshot snapshot;
		switch (Representation) {
		case xxr::xcsr_impl::Repr::CSR:
			snapshot = DoPopulationSnapshotCSR();
			break;

		case xxr::xcsr_impl::Repr::OBR:
			snapshot = DoPopulationSnapshotOBR();
			break;

		case xxr::xcsr_impl::Repr::UBR:
			snapshot = DoPopulationSnapshotUBR();
			break;

		default:
			assert(false);
		}

		if (record) {
			if (snapShots.size() >= MaxRecords)
				snapShots.erase(snapShots.begin());
			snapShots.push_back(snapshot);
		}

		if (snapshot.AverageFitness > m_maxFitness)
			m_maxFitness = snapshot.AverageFitness;

		if (std::abs(snapshot.AverageEpsilon) > m_maxEpsilon)
			m_maxEpsilon = std::abs(snapshot.AverageEpsilon);

		if (snapshot.AveragePrediction > m_maxPrediction) {
			m_maxPrediction = snapshot.AveragePrediction;
		}

		if (snapshot.ActionSetSize > m_maxActionSetSize)
			m_maxActionSetSize = snapshot.ActionSetSize;

		return snapshot;
	}

	void XCSAgentSupervisor::SetRecording(bool value)
	{
		m_recording = value;
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

	void XCSAgentSupervisor::SwitchCondensationMode(bool value)
	{
		if (value == m_isInCondensationMode) return;

		if (value) {
			m_preCondensationSnapshot.chi = GetConstants().chi;
			m_preCondensationSnapshot.mu = GetConstants().mu;
			m_preCondensationSnapshot.subsumptionTolerance = GetConstants().subsumptionTolerance;
			m_xcsr->switchToCondensationMode();
		}
		else {
			GetConstants().chi = m_preCondensationSnapshot.chi;
			GetConstants().mu = m_preCondensationSnapshot.mu;
			GetConstants().subsumptionTolerance = m_preCondensationSnapshot.subsumptionTolerance;
		}

		m_isInCondensationMode = value;
	}

	XCSAgentSupervisor::PopulationSnapshot XCSAgentSupervisor::DoPopulationSnapshotCSR() const
	{
		auto xcsrCS = dynamic_cast<const xxr::xcsr_impl::csr::Experiment<double, int>*>(&m_xcsr->GetExperiment());
		auto population = xcsrCS->GetPopulation();

		PopulationSnapshot snapshot;

		for (auto cl : population) {
			float accuracy = cl->accuracy();
			snapshot.AverageAccuracy += accuracy;
			snapshot.AverageEpsilon += std::abs(cl->epsilon);
			snapshot.AverageExperience += cl->experience;
			snapshot.AverageFitness += cl->fitness;
			snapshot.AveragePrediction += cl->prediction;
			snapshot.ActionSetSize += cl->actionSetSize;

			if (accuracy < snapshot.MinAccuracy)
				snapshot.MinAccuracy = accuracy;
			if (accuracy > snapshot.MaxAccuracy)
				snapshot.MaxAccuracy = accuracy;

			if (std::abs(cl->epsilon) < snapshot.MinEpsilon)
				snapshot.MinEpsilon = std::abs(cl->epsilon);
			if (std::abs(cl->epsilon) > snapshot.MaxEpsilon)
				snapshot.MaxEpsilon = std::abs(cl->epsilon);

			if (cl->fitness < snapshot.MinFitness)
				snapshot.MinFitness = cl->fitness;
			if (cl->fitness > snapshot.MaxFitness)
				snapshot.MaxFitness = cl->fitness;

			if (cl->prediction < snapshot.MinPrediction)
				snapshot.MinPrediction = cl->prediction;
			if (cl->prediction > snapshot.MaxPrediction)
				snapshot.MaxPrediction = cl->prediction;

			snapshot.NumerositySum += cl->numerosity;
		}

		snapshot.MacroClassifierCount = population.size();
		snapshot.ActionSetSize /= snapshot.MacroClassifierCount;
		snapshot.AverageAccuracy /= snapshot.MacroClassifierCount;
		snapshot.AverageEpsilon /= snapshot.MacroClassifierCount;
		snapshot.AverageExperience /= snapshot.MacroClassifierCount;
		snapshot.AverageFitness /= snapshot.MacroClassifierCount;
		snapshot.AveragePrediction /= snapshot.MacroClassifierCount;

		return snapshot;
	}

	XCSAgentSupervisor::PopulationSnapshot XCSAgentSupervisor::DoPopulationSnapshotUBR() const
	{
		auto xcsrUBR = dynamic_cast<const xxr::xcsr_impl::ubr::Experiment<double, int>*>(&m_xcsr->GetExperiment());
		auto population = xcsrUBR->GetPopulation();

		PopulationSnapshot snapshot;

		for (auto cl : population) {
			float accuracy = cl->accuracy();
			snapshot.AverageAccuracy += accuracy;
			snapshot.AverageEpsilon += std::abs(cl->epsilon);
			snapshot.AverageExperience += cl->experience;
			snapshot.AverageFitness += cl->fitness;
			snapshot.AveragePrediction += cl->prediction;
			snapshot.ActionSetSize += cl->actionSetSize;

			if (accuracy < snapshot.MinAccuracy)
				snapshot.MinAccuracy = accuracy;
			if (accuracy > snapshot.MaxAccuracy)
				snapshot.MaxAccuracy = accuracy;

			if (std::abs(cl->epsilon) < snapshot.MinEpsilon)
				snapshot.MinEpsilon = std::abs(cl->epsilon);
			if (std::abs(cl->epsilon) > snapshot.MaxEpsilon)
				snapshot.MaxEpsilon = std::abs(cl->epsilon);

			if (cl->fitness < snapshot.MinFitness)
				snapshot.MinFitness = cl->fitness;
			if (cl->fitness > snapshot.MaxFitness)
				snapshot.MaxFitness = cl->fitness;

			if (cl->prediction < snapshot.MinPrediction)
				snapshot.MinPrediction = cl->prediction;
			if (cl->prediction > snapshot.MaxPrediction)
				snapshot.MaxPrediction = cl->prediction;

			snapshot.NumerositySum += cl->numerosity;
		}

		snapshot.MacroClassifierCount = population.size();
		snapshot.ActionSetSize /= snapshot.MacroClassifierCount;
		snapshot.AverageAccuracy /= snapshot.MacroClassifierCount;
		snapshot.AverageEpsilon /= snapshot.MacroClassifierCount;
		snapshot.AverageExperience /= snapshot.MacroClassifierCount;
		snapshot.AverageFitness /= snapshot.MacroClassifierCount;
		snapshot.AveragePrediction /= snapshot.MacroClassifierCount;

		return snapshot;
	}

	XCSAgentSupervisor::PopulationSnapshot XCSAgentSupervisor::DoPopulationSnapshotOBR() const
	{
		auto xcsrOBR = dynamic_cast<const xxr::xcsr_impl::obr::Experiment<double, int>*>(&m_xcsr->GetExperiment());
		auto population = xcsrOBR->GetPopulation();

		PopulationSnapshot snapshot;

		for (auto cl : population) {
			float accuracy = cl->accuracy();
			snapshot.AverageAccuracy += accuracy;
			snapshot.AverageEpsilon += std::abs(cl->epsilon);
			snapshot.AverageExperience += cl->experience;
			snapshot.AverageFitness += cl->fitness;
			snapshot.AveragePrediction += cl->prediction;
			snapshot.ActionSetSize += cl->actionSetSize;

			if (accuracy < snapshot.MinAccuracy)
				snapshot.MinAccuracy = accuracy;
			if (accuracy > snapshot.MaxAccuracy)
				snapshot.MaxAccuracy = accuracy;

			if (std::abs(cl->epsilon) < snapshot.MinEpsilon)
				snapshot.MinEpsilon = std::abs(cl->epsilon);
			if (std::abs(cl->epsilon) > snapshot.MaxEpsilon)
				snapshot.MaxEpsilon = std::abs(cl->epsilon);

			if (cl->fitness < snapshot.MinFitness)
				snapshot.MinFitness = cl->fitness;
			if (cl->fitness > snapshot.MaxFitness)
				snapshot.MaxFitness = cl->fitness;

			if (cl->prediction < snapshot.MinPrediction)
				snapshot.MinPrediction = cl->prediction;
			if (cl->prediction > snapshot.MaxPrediction)
				snapshot.MaxPrediction = cl->prediction;

			snapshot.NumerositySum += cl->numerosity;
		}

		snapshot.MacroClassifierCount = population.size();
		snapshot.ActionSetSize /= snapshot.MacroClassifierCount;
		snapshot.AverageAccuracy /= snapshot.MacroClassifierCount;
		snapshot.AverageEpsilon /= snapshot.MacroClassifierCount;
		snapshot.AverageExperience /= snapshot.MacroClassifierCount;
		snapshot.AverageFitness /= snapshot.MacroClassifierCount;
		snapshot.AveragePrediction /= snapshot.MacroClassifierCount;

		return snapshot;
	}
}