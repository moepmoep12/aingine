#pragma once
#include "AIngine/Core.h"
#include "Forklift.h"
#include "Tasks.h"
#include "Graph.h"

namespace Forklifts {
	class Experiment : public AIngine::Script {
	public:
		Experiment();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;
		virtual void OnGUI() override;

		const int forkLiftCount = 4;

		int AmountCollisions = 0;

	private:
		void Init();
		void OnForkliftTaskFinished(Forklift* forklift);

	private:
		std::vector<std::pair<Forklift*, std::vector<Task>>> m_forkLiftTasks;
		Graph* m_graph;
		AIngine::Events::EventHandler<void> OnGraphLoadedEventHandler;
		Forklift::OnTaskFinishedEventHandler TaskFinishedHandler;
	};
}