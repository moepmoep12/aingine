#pragma once
#include "AIngine/Script.h"

namespace CrappyBird {
	class BackGround : public AIngine::Script {
	public:
		BackGround(AIngine::GameObject* owner);
		virtual void OnStart() override;
		virtual void OnEvent(AIngine::Events::EventData& e) override;
		virtual void Update(float deltatime) override;
		virtual ~BackGround();
	};
}