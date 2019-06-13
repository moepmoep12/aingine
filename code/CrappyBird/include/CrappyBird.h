#pragma once
#include "AIngine/Core.h"


namespace CrappyBird {

	static float s_GameSpeed = 0.5f;

	class CrappyBird : public AIngine::Application {
	public:

		// Inherited via Application
		virtual void OnAppStartUp() override {}
		virtual void OnAppShutDown() override {}
		virtual void OnAppUpdate() override {}
		virtual void OnAppEvent(AIngine::Events::EventData& e) override {}

		CrappyBird();
		virtual ~CrappyBird();
	};
}