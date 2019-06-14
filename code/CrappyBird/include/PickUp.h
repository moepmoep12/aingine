#pragma once
#include "AIngine/Script.h"
#include "AIngine/Physics.h"

namespace CrappyBird {
	class PickUp : public AIngine::Script {
	public:
		PickUp(AIngine::GameObject* owner);
		virtual void OnStart() override;

		virtual void OnPickUp() {}

	public:
		bool Active = false;

	private:
		void OnCollision(AIngine::Physics::PhysicsComponent* other);
	};


	class SpeedPickUp : public PickUp {
	public:
		SpeedPickUp(AIngine::GameObject* owner) : PickUp(owner) {}
		virtual void OnPickUp() override;
		virtual void Update(float deltatime) override;

		float Duration = 5;
	};
}