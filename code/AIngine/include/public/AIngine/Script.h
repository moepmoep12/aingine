#pragma once

#include "AIngine/Component.h"
#include "Application.h"
#include <typeinfo>

namespace AIngine {

	namespace Structures {
		class UpdateTraverser;
		class OnStartTraverser;
	}

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

		/* OnEvent propagates events*/
		virtual void OnEvent(AIngine::Events::EventData& e) {}

		virtual void OnGUI() {}

		/* Index used to uniquely identify a script and for serialization */
		int ScriptIndex = 0;

	private:
		virtual void OnUpdate(float deltatime) override { if (AIngine::Application::IsRunning()) Update(deltatime); }
		virtual void OnImguiRender() { if (AIngine::Application::IsRunning())  OnGUI(); }

		inline virtual Component* Copy(GameObject* const owner) const override
		{
			Script* copy = new Script();
			copy->m_owner = owner;
			copy->ScriptIndex = ScriptIndex;
			copy->SetName(GetName());
			return std::move(copy);
		}

		/* In order to call the start method during runtime ( if a component has been dynamically added),
		the UpdateTraverser will check whether the component already called Start */
		friend class AIngine::Structures::UpdateTraverser;
		friend class AIngine::Structures::OnStartTraverser;

		bool m_startCalled = false;
	};
}