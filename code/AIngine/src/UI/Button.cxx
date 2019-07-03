#include "UI/Button.h"
#include "Rendering/shader.h"
#include "AIngine/Graphics.h"

namespace AIngine::UI {
	bool Button::Render(const glm::mat4 & modelMatrix, AIngine::Rendering::GLShaderProgram & shader) const
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
			auto rectangle = GetRectangle();
			float xpos = rectangle.x + ((float)rectangle.width - textSize.x) * 0.5f;
			float ypos = rectangle.y + ((float)rectangle.height - textSize.y) * 0.5f;

			AIngine::Graphics::Text(Text, glm::vec2(xpos, ypos) + TextOffset, TextScale, TextColor, TextColor.w);
		}
		return true;
	}
	void Button::OnMouseReleased()
	{
		OnClickedEvent();
	}

	Component * Button::Copy(GameObject * const owner) const
	{
		Button* copy = new Button();
		copy->SetEnabled(IsEnabled());
		copy->m_owner = owner;
		copy->m_isDisabled = m_isDisabled;
		copy->DisabledColor = DisabledColor;
		copy->TintColor = TintColor;
		copy->AnchorPos = AnchorPos;
		copy->SetRectangle(GetRectangleNative());

		copy->ClickedColor = ClickedColor;
		copy->HoverColor = HoverColor;
		copy->Text = Text;
		copy->Texture = Texture;
		copy->TextScale = TextScale;
		copy->TextOffset = TextOffset;
		copy->TextColor = TextColor;

		return copy;
	}
}