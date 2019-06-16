#pragma once
#include "AIngine/Core.h"


namespace CrappyBird {


	class CrappyBird : public AIngine::Application {
	public:

		// Inherited via Application
		virtual void OnAppStartUp() override {}
		virtual void OnAppShutDown() override {}
		virtual void OnAppUpdate() override {}
		virtual void OnAppEvent(AIngine::Events::EventData& e) override {}

		static float s_GameSpeed;
		const static float s_originalGameSpeed;

		CrappyBird();
		virtual ~CrappyBird();
	};
}