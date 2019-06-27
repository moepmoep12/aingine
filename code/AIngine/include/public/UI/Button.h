#pragma once
#include "UI/UIELement.h"
#include "Events/Event.h"
#include "Rendering/texture.h"

namespace AIngine::UI {
	class Button : public UIElement {
	public:
		glm::vec4 HoverColor = glm::vec4(1);
		glm::vec4 ClickedColor = glm::vec4(0.97,1,0.4,1);

		AIngine::Events::Event<void> OnClickedEvent;
		typedef AIngine::Events::EventHandler<void> OnClickedEventHandler;

		std::string Text;

		AIngine::Rendering::Texture2D Texture;

		virtual void OnImGuiRender() override;

	protected:
		virtual void OnClicked() override;

	};
}