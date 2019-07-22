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

		enum {
			MaxRecords = 10000
		};

		struct PopulationSnapshot {
			float NumerositySum = 0;
			float MacroClassifierCount = 0;

			float MinFitness = std::numeric_limits<float>::max();
			float MaxFitness = std::numeric_limits<float>::lowest();
			float AverageFitness = 0;

			float MinAccuracy = std::numeric_limits<float>::max();
			float MaxAccuracy = std::numeric_limits<float>::lowest();
			float AverageAccuracy = 0;

			float MinEpsilon = std::numeric_limits<float>::max();
			float MaxEpsilon = std::numeric_limits<float>::lowest();
			float AverageEpsilon = 0;

			float MinPrediction = std::numeric_limits<float>::max();
			float MaxPrediction = std::numeric_limits<float>::lowest();
			float AveragePrediction = 0;

			float AverageExperience = 0;

			float ActionSetSize = 0;

		};

		struct EpisodeSnapshot {
			float Index = 0;
			float DurationInTicks = 0;
			float RewardSum = 0;
		};

		XCSAgentSupervisor();
		void ChangeRepresentation(xxr::xcsr_impl::Repr repr);

		xxr::xcsr_impl::Repr GetRepresentation() const { return Representation; }

		PopulationSnapshot GetPopulationSnapShot(bool record = true);
		inline const std::vector<PopulationSnapshot>& GetSnapShots() const { return snapShots; }

		inline const std::vector<EpisodeSnapshot>& GetEpisodeSnapshots() const { return episodes; }

		inline float GetMaxReward() const { return m_maxReward; }
		inline int GetMaxDuration() const { return m_maxDuration; }
		inline float GetMaxFitness() const { return m_maxFitness; }
		inline float GetMaxEpsilon() const { return m_maxEpsilon; }
		inline float GetMaxPrediction() const { return m_maxPrediction; }
		inline float GetMaxActionSetSize() const { return m_maxActionSetSize; }

		bool IsRecording() const { return m_recording; }
		void SetRecording(bool value);

		int GetRecordInterval() const { return m_recordInterval; }
		inline void SetRecordInterval(int value) { m_recordInterval = value; }

		xxr::XCSRConstants& GetConstants();

		std::string PopulationPath;

		void SwitchCondensationMode(bool value);
		inline bool IsInCondensationMode() const { return m_isInCondensationMode; }

	public:
		std::unique_ptr<xxr::xcsr_impl::Experiment<double, int>> m_xcsr;
		std::unordered_set<int> PossibleActions = { 0,1 };
		bool Exploration = true;

	private:
		xxr::xcsr_impl::Repr Representation = xxr::xcsr_impl::Repr::CSR;
		bool m_isInCondensationMode = false;
		bool m_recording = false;
		int m_recordInterval = 0;
		int m_recordTimer = 0;
		int m_maxDuration = 0;
		float m_maxReward = 0;
		float m_maxFitness = 0;
		float m_maxEpsilon = 0;
		float m_maxPrediction = 0;
		float m_maxActionSetSize = 0;
		std::vector<PopulationSnapshot> snapShots;
		std::vector<EpisodeSnapshot> episodes;

		struct PreCondensationSnapshot {
			float chi;
			float mu;
			float subsumptionTolerance;
		};

		PreCondensationSnapshot m_preCondensationSnapshot;

	private:
		PopulationSnapshot DoPopulationSnapshotCSR() const;
		PopulationSnapshot DoPopulationSnapshotUBR() const;
		PopulationSnapshot DoPopulationSnapshotOBR() const;
	};
}