#include "UI/Image.h"
#include "Rendering/shader.h"

namespace AIngine::UI {

	bool Image::Render(const glm::mat4 & modelMatrix, AIngine::Rendering::GLShaderProgram & shader) const
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
		copy->SetEnabled(IsEnabled());
		copy->m_owner = owner;
		copy->m_isDisabled = m_isDisabled;
		copy->DisabledColor = DisabledColor;
		copy->TintColor = TintColor;
		copy->AnchorPos = AnchorPos;
		copy->SetRectangle(GetRectangleNative());

		copy->Texture = Texture;
		return copy;
	}
}