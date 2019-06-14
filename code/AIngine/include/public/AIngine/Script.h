#pragma once

#include "AIngine/Component.h"
#include "Application.h"
#include <typeinfo>

namespace AIngine {
	class Script : public Component {
	public:
		Script(GameObject* owner) : Component(owner) { SetName(typeid(*this).name()); }
		virtual ~Script() {}

		virtual void OnStart() {}
		virtual void Update(float deltatime) {}
		virtual void OnEnd() {}

		virtual void OnEvent(AIngine::Events::EventData& e) {}
		virtual void OnImguiRender() {}

		int ScriptIndex = 0;
		virtual void OnUpdate(float deltatime) override { if (AIngine::Application::IsRunning()) Update(deltatime); }
	};
}