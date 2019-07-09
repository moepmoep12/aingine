#pragma once

#include"AIngine/Core.h"

namespace Pong {
	class Pong : public AIngine::Application {
	public:
		Pong();
		// Inherited via Application
		virtual void OnAppStartUp() override;
		virtual void OnAppShutDown() override;
		virtual void OnAppUpdate() override;
		virtual void OnAppEvent(AIngine::Events::EventData& e) override;
	};
}