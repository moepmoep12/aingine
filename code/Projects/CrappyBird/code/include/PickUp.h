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
		void OnCollisionBegin(AIngine::Physics::Contact contact);
		void OnCollisionEnd(AIngine::Physics::Contact contact);

		/* EventHandler*/
		AIngine::Physics::OnCollisionHandler m_collisionBeginHandler;
		AIngine::Physics::OnCollisionHandler m_collisionEndHandler;

		Player* m_player;
	};
}