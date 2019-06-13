#pragma once

#include "AIngine/Component.h"
#include "Application.h"
#include <typeinfo>

namespace AIngine {
	class Script : public Component {
	public:
		Script(GameObject* owner) : Component(owner) { SetName(typeid(this).name()); }
		virtual ~Script() {}
		virtual void OnStart() {}
		virtual void OnEvent(AIngine::Events::EventData& e) {}
		virtual void OnUpdate(float deltatime) { if (AIngine::Application::IsRunning()) Update(deltatime); }
		virtual void Update(float deltatime) {}
		virtual void OnImguiRender() {}

		int ScriptIndex = 0;
	};
}