#pragma once
#include "AIngine/Core.h"


class Game : public AIngine::Application {
public:
	Game();

	~Game();

	// Inherited via Application
	virtual void OnAppStartUp() override;
	virtual void OnAppShutDown() override;
	virtual void OnAppUpdate() override;


	//Texture2D* texture;
};