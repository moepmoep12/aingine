#pragma once
#include "UI/UIELement.h"
#include "Rendering/texture.h"


namespace AIngine::UI {
	class Image : public UIElement {
	public:
		AIngine::Rendering::Texture2D Texture;

		virtual void Render(const glm::mat4& modelMatrix, AIngine::Rendering::GLShaderProgram& shader) const override;
	};
}