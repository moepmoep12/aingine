#pragma once
#include "AIngine/Script.h"
#include "AIngine/GameObject.h"
#include "AIngine/Physics.h"
#include "PickUp.h"

namespace CrappyBird {
	class Player : public AIngine::Script
	{
	public:

		Player();
		virtual void OnStart() override;
		virtual void Update(float deltatime) override;
		virtual void OnEnd() override;
		virtual void OnEvent(AIngine::Events::EventData& e) override;
		virtual void OnGUI() override;
		virtual ~Player();

		AIngine::Events::Event<void> OnEnterNewScreen;

		int CurrentScreenIndex = 1;

	private:
		AIngine::Physics::PhysicsComponent* m_physBody;
		PickUpFactory* m_pickUpFactory;
		float m_originalGameSpeed = 0;
		float m_distanceTraveled = 0;
		glm::vec2 m_spawnPos;
	};
}