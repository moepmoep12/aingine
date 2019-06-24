#pragma once
#include "AIngine/Core.h"
#include "PickUp.h"

namespace CrappyBird {
	class PickUpFactory : public AIngine::Script {
	public:
		PickUpFactory();

		void SpawnPickUp(const glm::vec2& pos);

	private:
		GameObject* GetAvailableGameObject();
		void AddEffect(PickUp& obj);
	};
}