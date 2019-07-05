#include "UI/Image.h"
#include "Rendering/shader.h"

namespace AIngine::UI {

	bool Image::Render(AIngine::Rendering::GLShaderProgram & shader) const
	{
		Texture.Bind();
		glm::vec4 color = TintColor;
		if (IsDisabled())
			color = DisabledColor;

		shader.SetVector4f(5 /*spriteColor*/, color);
		return true;
	}
	Component * Image::Copy(GameObject * const owner) const
	{
		Image* copy = new Image();

		//  Component 
		copy->SetEnabled(IsEnabled());
		copy->m_owner = owner;
		copy->PostInit();

		// UIElement
		copy->m_isDisabled = m_isDisabled;
		copy->DisabledColor = DisabledColor;
		copy->TintColor = TintColor;
		copy->SetAnchor(AnchorPos);
		copy->SetRectangle(GetRectangleNative());

		copy->Texture = Texture;
		return copy;
	}
}