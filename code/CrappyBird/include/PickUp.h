#pragma once
#include "AIngine/Core.h"
#include "Effects.h"
#include "Player.h"

#include <vector>

namespace CrappyBird {


	class PickUp : public Script {
	public:
		PickUp();

		// inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float deltatime) override;


		Effect m_Effect;

	private:
		/* Callback*/
		void OnCollision(PhysicsComponent* other);

		/* EventHandler*/
		AIngine::Events::EventHandler<void, PhysicsComponent*> m_collisionHandler;

		Player* m_player;

	};

	class PickUpFactory : public Script {
	public:
		PickUpFactory();

		// inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;

		void SpawnPickUp(const glm::vec2& pos);

	private:
		GameObject* GetAvailableGameObject();
		void AddEffect(PickUp& obj);
	};

}