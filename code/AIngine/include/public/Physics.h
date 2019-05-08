#include "Component.h"

namespace AIngine {

	class PhysicsComponent : public AIngine::Component {
	public:
		PhysicsComponent(GameObject* owner);
		virtual ~PhysicsComponent();

		// inherited via Component
		virtual void OnUpdate(float deltatime) override;

	};
}