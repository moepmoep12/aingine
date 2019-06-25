#pragma once
#include "AIngine/Core.h"
#include "xxr/xcsr.hpp"
#include "Obstacle.h"

namespace CrappyBird {
	class Agent : public AIngine::Script {
	public:
		Agent();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;
		virtual void OnGUI() override;

		xxr::XCSR<>* xcsr;

	private:
		// the physical body of the rocket
		AIngine::Physics::PhysicsComponent* m_physBody;
		std::vector<GameObject*> obstacles;
	};

}