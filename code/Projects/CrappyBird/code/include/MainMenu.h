#pragma once
#include "AIngine/Core.h"

namespace CrappyBird {
	class MainMenu : public AIngine::Script {
	public:
		MainMenu();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;
		virtual void OnGUI() override;
	};
}