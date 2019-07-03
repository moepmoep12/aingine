#pragma once
#include "UI/UIELement.h"
#include "Rendering/texture.h"
#include "Events/Event.h"
#include "UI/Text.h"

namespace AIngine::UI {
	class CheckBox : public UIElement {
	public:
		CheckBox();
		virtual bool Render(const glm::mat4& modelMatrix, AIngine::Rendering::GLShaderProgram& shader) const override;

		virtual void PostInit() override;

		inline bool GetState() const { return m_bState; }
		inline void SetState(bool state) {
			if (state != m_bState) {
				m_bState = state;
				OnStateChangedEvent(state);
		} }

		AIngine::Events::Event<void,bool> OnStateChangedEvent;
		typedef AIngine::Events::EventHandler<void,bool> OnStateChangedEventHandler;

		UIText* TextComponent;

	private:
		virtual void OnClicked() override;

	private:
		AIngine::Rendering::Texture2D TextureOn;
		AIngine::Rendering::Texture2D TextureOff;
		bool m_bState = false;

	protected:
		virtual Component* Copy(GameObject* const owner) const override;
	};
}