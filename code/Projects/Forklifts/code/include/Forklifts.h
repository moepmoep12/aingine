#pragma once

#include"AIngine/Core.h"

namespace Forklifts {
	class Forklifts : public AIngine::Application {
	public:
		Forklifts();
		// Inherited via Application
		virtual void OnAppStartUp() override;
		virtual void OnAppShutDown() override;
		virtual void OnAppUpdate() override;
		virtual void OnAppEvent(AIngine::Events::EventData& e) override;
	};
}