#pragma once
#include "AIngine/Core.h"

namespace Forklifts {
	class Forklift : public AIngine::Script {
	public:
		Forklift();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		int currentTaskIndex = 0;

	};
}