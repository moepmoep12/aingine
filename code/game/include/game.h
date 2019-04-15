#pragma once
#include "Core.h"


class Game : public AIngine::Application {
public:
	Game();

	~Game();

	// Inherited via Application
	virtual void OnAppStartUp() override;
	virtual void OnAppShutDown() override;
};