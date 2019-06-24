#pragma once
#include "AIngine/Core.h"
#include "Effects.h"
#include "Player.h"

#include <memory>

namespace CrappyBird {
	class PickUp : public AIngine::Script {
	public:
		PickUp();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		std::unique_ptr<Effect> m_Effect;

	private:
		/* Callback*/
		void OnCollision(PhysicsComponent* other);

		/* EventHandler*/
		AIngine::Events::EventHandler<void, PhysicsComponent*> m_collisionHandler;

		Player* m_player;
	};
}