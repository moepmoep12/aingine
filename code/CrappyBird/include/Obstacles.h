#pragma once

#include "AIngine/Script.h"

namespace CrappyBird {
	class Obstacles : public AIngine::Script {
		Obstacles(AIngine::GameObject* owner);
		virtual void OnStart() override;
		virtual void OnEvent(AIngine::Events::EventData& e) override;
		virtual void Update(float deltatime) override;
		virtual ~Obstacles();

	private:
		glm::vec2 m_playerSpawnPos;
	};
}