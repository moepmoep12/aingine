#pragma once

#include "AIngine/Component.h"
#include "Application.h"
#include "Events/ApplicationEvents.h"

#include <typeinfo>

namespace AIngine {

	class Script : public Component {
	public:
		Script() { SetName(typeid(*this).name()); }
		virtual ~Script() {}

		/* Start is called after the scene was initialized and before the first frame*/
		virtual void OnStart() {}

		/* Update is called once per frame */
		virtual void Update(float deltatime) {}

		/* End is called before the scene shuts down*/
		virtual void OnEnd() {}

		virtual void OnEventData(AIngine::Events::EventData& e) {}

		/* Here should rendering happen */
		virtual void OnGUI() {}

		/* Custom editor widgets can be created here */
		virtual void OnWidget() {}

		/* Index used to uniquely identify a script and for serialization */
		int ScriptIndex = 0;

	private:
		inline virtual void OnUpdate(float deltatime) override
		{
			if (AIngine::Application::IsRunning())
			{
				// if this component was added during runtime we need to manually call OnStart()
				if (!m_startCalled) {
					m_startCalled = true;
					OnStart();
				}
				Update(deltatime);
			}
		}
		inline virtual void OnImGuiRender() override { if (AIngine::Application::IsRunning())  OnGUI(); }

		/* OnEvent propagates events*/
		inline virtual void OnEvent(AIngine::Events::EventData& e) override
		{
			if (typeid(e) == typeid(AIngine::Events::EnterPlayModeEventData)) {
				if (!m_startCalled) {
					m_startCalled = true;
					OnStart();
					return;
				}
			}
			if (typeid(e) == typeid(AIngine::Events::ExitPlayModeEventData)) {
				OnEnd();
				return;
			}

			// Don't propagte events if we're in Editor
			if (!AIngine::Application::IsRunning() &&
				(dynamic_cast<AIngine::Events::KeyEvent::KeyEventData*>(&e)
					|| dynamic_cast<AIngine::Events::MouseButtonEvent::MouseButtonEventData*>(&e)
					|| dynamic_cast<AIngine::Events::MouseMovedEvent::MouseMovedEventData*>(&e)
					|| dynamic_cast<AIngine::Events::MouseScrolledEvent::MouseScrolledEventData*>(&e))) {
				return;
			}

			// propagate the event to subclass
			OnEventData(e);
		}

		inline virtual Component* Copy(GameObject* const owner) const override
		{
			Script* copy = new Script();

			// generic Component 
			copy->SetEnabled(IsEnabled());
			copy->m_owner = owner;
			copy->PostInit();

			copy->ScriptIndex = ScriptIndex;
			copy->SetName(GetName());
			return std::move(copy);
		}

		bool m_startCalled = false;
	};
}