#pragma once
#include "AIngine/Core.h"

#include <vector>

namespace CrappyBird {


	class PickUp : public Script {
	public:
		PickUp() { SetName(typeid(*this).name()); }
		virtual void OnStart() override;
		virtual void Update(float deltatime) override;

		virtual void OnPickUp() {}

	public:
		bool Active = false;
		bool Available = true;

	protected:
		void OnFinish();

	private:
		void OnCollision(PhysicsComponent* other);
		AIngine::Events::EventHandler<void, PhysicsComponent*> m_collisionHandler;
	};

	class PickUpFactory : public Script {
	public:
		PickUpFactory() { SetName(typeid(*this).name()); }
		virtual void OnStart() override;

		void SpawnPickUpInArea(const AIngine::Structures::RectangleI& worldRect);

	private:
		GameObject* GetAvailableGameObject();
	};

	class SpeedPickUp : public PickUp {
	public:
		SpeedPickUp() { SetName(typeid(*this).name()); }
		virtual void OnPickUp() override;
		virtual void Update(float deltatime) override;

		float Duration = 5;
	};

	class SlowSpeedPickUp : public PickUp {
	public:
		SlowSpeedPickUp() { SetName(typeid(*this).name()); }
		virtual void OnPickUp() override;
		virtual void Update(float deltatime) override;

		float Duration = 5;
	};
}