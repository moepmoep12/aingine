#pragma once

#include"AIngine/Core.h"

namespace CrappyBird {
	class CrappyBird : public AIngine::Application {
	public:
		CrappyBird();
		// Inherited via Application
		virtual void OnAppStartUp() override;
		virtual void OnAppShutDown() override;
		virtual void OnAppUpdate() override;
		virtual void OnAppEvent(AIngine::Events::EventData& e) override;

		// GameSettings
		static float s_GameSpeed;
		static bool s_bObstacleRotation;
		static int s_levelIndex;
		static bool s_DieOnCollision;
		static bool s_AgentLearning;
		static float s_Impulse;

		const static float s_originalGameSpeed;
		/* The impulse the rocket receives on button press*/
		static const float s_standardImpulse;
	};
}