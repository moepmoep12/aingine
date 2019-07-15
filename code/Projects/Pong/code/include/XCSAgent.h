#pragma once
#include "AIngine/Core.h"
#include "AIngine/Agent.h"

namespace Pong {
	class XCSAgent : public AIngine::Agent {
	public:
		XCSAgent();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		// Inherited via Agent
		virtual std::vector<double, std::allocator<double>> Observe();
		virtual bool IsEndOfProblem();
		virtual void ExecuteAction(int action);
		virtual void Reset();
		virtual void OnMaxStepsReached();
	};
}