#pragma once
#include "AIngine/Core.h"
#include "Player.h"

namespace Pong {
	class RandomAgent : public Player {
	public:
		RandomAgent();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		virtual void ReceiveBall() override;

	private:
		bool receivedBall = false;
	};
}