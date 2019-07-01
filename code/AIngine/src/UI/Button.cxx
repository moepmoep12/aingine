#include "UI/Button.h"
#include "Rendering/shader.h"
#include "AIngine/Graphics.h"

namespace AIngine::UI {
	void Button::Render(const glm::mat4 & modelMatrix, AIngine::Rendering::GLShaderProgram & shader) const
	{
		Texture.Bind();
		glm::vec4 color = TintColor;
		if (IsDisabled())
			color = DisabledColor;

		else if (m_isClicked)
			color = ClickedColor;
		else
			if (IsHovered())
				color = HoverColor;

		shader.SetVector4f(5 /*spriteColor*/, color);

		if (!Text.empty()) {
			glm::vec2 textSize = AIngine::Graphics::GetTextSize(Text, TextScale);

			float xpos = m_rectangle.x + ((float)m_rectangle.width - textSize.x) * 0.5f;
			float ypos = m_rectangle.y + ((float)m_rectangle.height - 2 * textSize.y);

			AIngine::Graphics::Text(Text, glm::vec2(xpos, ypos) + TextOffset, TextScale, TextColor, TextColor.w);
		}
	}
	void Button::OnMouseReleased()
	{
		OnClickedEvent();
	}
}