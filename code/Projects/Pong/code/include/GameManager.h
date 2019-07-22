#pragma once
#include "AIngine/Core.h"

namespace Pong {
	class GameManager : public AIngine::Script {
	public:
		GameManager();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;
		virtual void OnWidget() override;

		//static glm::vec2 ForceOnBall;
	};
}