#pragma once
#include "AIngine/Core.h"

class Launcher : public AIngine::Application {
public:
	Launcher();

	// Inherited via Application
	virtual void OnAppStartUp() override {}
	virtual void OnAppShutDown() override {}
	virtual void OnAppUpdate() override {}
	virtual void OnAppEvent(AIngine::Events::EventData& e) override {}

};