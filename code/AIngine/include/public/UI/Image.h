#pragma once
#include "UI/UIELement.h"
#include "Rendering/texture.h"


namespace AIngine::UI {
	class Image : public UIElement {
	public:
		AIngine::Rendering::Texture2D Texture;

		virtual bool Render(AIngine::Rendering::GLShaderProgram& shader) const override;

	protected:
		virtual Component* Copy(GameObject* const owner) const override;
	};
}