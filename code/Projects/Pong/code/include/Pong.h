#pragma once

#include"AIngine/Core.h"
#include "UI/Text.h"

namespace Pong {
	enum class PlayerRole {
		One,
		Two
	};

	class Pong : public AIngine::Application {
	public:
		Pong();
		// Inherited via Application
		virtual void OnAppStartUp() override;
		virtual void OnAppShutDown() override;
		virtual void OnAppUpdate() override;
		virtual void OnAppEvent(AIngine::Events::EventData& e) override;

		static int Difficulty;

		static std::map<int, float> PlayerSizes;
	};
}