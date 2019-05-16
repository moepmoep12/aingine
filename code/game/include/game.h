#pragma once
#include "AIngine/Core.h"
#include "Rendering/bitmap.h"

class Game : public AIngine::Application {
public:
	Game();

	~Game();

	// Inherited via Application
	virtual void OnAppStartUp() override;
	virtual void OnAppShutDown() override;
	virtual void OnAppUpdate() override;
	virtual void OnAppEvent(AIngine::Events::Event& e) override;

	AIngine::GameObject* SpawnObjectAtMousePosition(const glm::vec2& minSize, const glm::vec2& maxSize,  Bitmap& bitmap);
};