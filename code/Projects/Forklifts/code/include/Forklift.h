#pragma once
#include "AIngine/Core.h"

namespace Forklifts {
	class Forklift : public AIngine::Script {
	public:
		Forklift();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

	private:
		virtual void OnCollision(AIngine::Physics::Contact contact);
		AIngine::Events::EventHandler<void, AIngine::Physics::Contact> OnCollisionEventHandler;

		int currentTaskIndex = 0;

	};
}