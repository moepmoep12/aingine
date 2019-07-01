#pragma once
#include "UI/UIELement.h"
#include "Events/Event.h"
#include "Rendering/texture.h"


namespace AIngine::UI {
	class Button : public UIElement {
	public:
		glm::vec4 HoverColor = glm::vec4(1);
		glm::vec4 ClickedColor = glm::vec4(0.97, 1, 0.4, 1);

		AIngine::Events::Event<void> OnClickedEvent;
		typedef AIngine::Events::EventHandler<void> OnClickedEventHandler;

		std::string Text;
		glm::vec4 TextColor = glm::vec4(0,0,0,1);
		glm::vec2 TextOffset = glm::vec2(0);
		glm::vec2 TextScale = glm::vec2(1);

		AIngine::Rendering::Texture2D Texture;

		virtual void Render(const glm::mat4& modelMatrix, AIngine::Rendering::GLShaderProgram& shader) const override;


	protected:
		virtual void OnMouseReleased() override;
	};
}