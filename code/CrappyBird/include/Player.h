#pragma once
#include "AIngine/Script.h"
#include "AIngine/GameObject.h"
#include "AIngine/Physics.h"

namespace CrappyBird {
	class Player : public AIngine::Script
	{
	public:
		Player(AIngine::GameObject* owner);
		virtual void OnStart() override;
		virtual void OnEvent(AIngine::Events::EventData& e) override;
		virtual void Update(float deltatime) override;
		virtual ~Player();

	private:
		AIngine::Physics::PhysicsComponent* m_physBody;
	};
}