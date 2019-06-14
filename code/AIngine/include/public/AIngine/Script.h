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

	private:
		virtual void OnUpdate(float deltatime) override { if (AIngine::Application::IsRunning()) Update(deltatime); }

		inline virtual Component* Copy(GameObject* const owner) const override
		{
			Script* copy = new Script(owner);
			copy->ScriptIndex = ScriptIndex;
			copy->SetName(GetName());
			return std::move(copy);
		}
	};
}