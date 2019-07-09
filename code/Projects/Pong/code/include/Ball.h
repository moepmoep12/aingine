#pragma once
#include "AIngine/Core.h"
#include "Player.h"

namespace Pong {
	class Ball : public AIngine::Script {
	public:
		Ball();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		PhysicsComponent* LeftEdge;
		PhysicsComponent* RightEdge;
		Player* PlayerOne;
		Player* PlayerTwo;

	private:
		void OnCollisionLeft(AIngine::Physics::Contact contact);
		void OnCollisionRight(AIngine::Physics::Contact contact);

	};
}