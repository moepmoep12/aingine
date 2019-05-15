#pragma once
#include "AIngine/Component.h"
#include <Box2D/Box2D.h>

namespace AIngine {

	class PhysicsComponent : public AIngine::Component {
	public:
		PhysicsComponent(GameObject* owner);
		virtual ~PhysicsComponent();

		// inherited via Component
		virtual void OnUpdate(float deltatime) override;

	private:
		b2Body* m_body;

	};
}