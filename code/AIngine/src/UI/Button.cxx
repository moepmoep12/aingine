#include "UI/Button.h"
#include "Rendering/shader.h"

namespace AIngine::UI {
	void Button::Render(const glm::mat4 & modelMatrix, AIngine::Rendering::GLShaderProgram & shader) const
	{
		Texture.Bind();
		glm::vec4 color = TintColor;
		if (m_isClicked)
			color = ClickedColor;
		else
			if (IsHovered())
				color = HoverColor;

		shader.SetVector4f(5 /*spriteColor*/, color);
	}
	void Button::OnClicked()
	{
		OnClickedEvent();
	}
}