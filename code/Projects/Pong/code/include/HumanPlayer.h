#pragma once
#include "AIngine/Core.h"
#include "Player.h"

namespace Pong {
	class HumanPlayer : public Player {
	public:
		HumanPlayer();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;
	};
}