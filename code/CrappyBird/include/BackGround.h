#pragma once
#include "AIngine/Script.h"

namespace CrappyBird {
	class BackGround : public AIngine::Script {
	public:
		BackGround();
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float deltatime) override;
		virtual ~BackGround();
	};
}