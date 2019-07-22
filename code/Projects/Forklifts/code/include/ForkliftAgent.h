#pragma once
#include "AIngine/Core.h"
#include "Forklift.h"
#include "AIngine/Agent.h"
#include "Experiment.h"

namespace Forklifts {
	class ForkliftAgent : public Forklift, public AIngine::Agent {
	public:
		ForkliftAgent();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;
		virtual void OnWidget() override;

		// Inherited via Agent
		virtual std::vector<double> Observe() override;
		virtual bool IsEndOfProblem() override;
		virtual void ExecuteAction(int action) override;
		virtual void Reset() override;
		virtual void OnMaxStepsReached() override;
		virtual inline std::string Name() const override { return "ForkLiftAgent"; }

		// Inherited via Forklift
		virtual void OnNodeReached() override;
		virtual void FinishTask() override;
		virtual void ProcessTask()  override;

	private:
		virtual void OnCollisionBegin(AIngine::Physics::Contact contact) override;

	private:
		bool bTaskFinished = false;
		Experiment* m_experiment;
	};
}