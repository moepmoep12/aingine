#include "UI/Text.h"
#include "Rendering/shader.h"
#include "AIngine/Graphics.h"
#include "Assets/Assets.h"
#include "Application.h"
#include "Rendering/Viewport.h"

namespace AIngine::UI {


	UIText::UIText()
	{
		// load default
		static const std::string defaultFontPath = "AIngine/fonts/arial.ttf";
		std::string path = AIngine::Assets::GetFontPath(defaultFontPath, FontSize);
		m_currentFont = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::FontAsset>(path)->GetFont();
		m_fonts[FontSize] = m_currentFont;
	}

	bool UIText::Render(AIngine::Rendering::GLShaderProgram & shader) const
	{
		const AIngine::Rendering::Viewport& viewport = AIngine::Application::GetViewport();
		// Scale the text to look the same on different resolutions
		glm::vec2 scale = glm::vec2((float)viewport.GetViewportWidth() / (float)TargetResolution.x, (float)viewport.GetViewportHeight() / (float)TargetResolution.y);
		auto rect = GetRectangle();
		glm::vec2 pos = rect.GetPosition();
		glm::vec2 textSize = AIngine::Graphics::GetTextSize(Text, scale, m_fonts.at(FontSize));

		AlignTextHorizontally(&pos.x, textSize.x, glm::vec2(rect.x, rect.GetMax().x), AlignHorizontal);
		AlignTextVertically(&pos.y, textSize.y, glm::vec2(rect.y, rect.GetMax().y), AlignVertical);

		AIngine::Graphics::TextInstant(Text, pos, scale, TintColor, TintColor.w, m_fonts.at(FontSize));
		shader.Use();
		return false;
	}

	void UIText::ChangeFontSize(unsigned int size)
	{
		// we need to load the font
		if (m_fonts.find(size) == m_fonts.end())
		{
			std::string path = AIngine::Assets::GetFontPath(m_currentFont->Path, size);
			m_fonts[size] = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::FontAsset>(path)->GetFont();
			m_currentFont = m_fonts[size];
		}

		FontSize = size;
	}

	void UIText::SetFont(AIngine::Rendering::Font * font)
	{
		if (font != m_currentFont) {
			m_fonts.clear();
			m_fonts[font->Size] = font;
			m_currentFont = font;
			FontSize = font->Size;
		}
	}
	glm::vec2 UIText::GetTextSize() const
	{
		const AIngine::Rendering::Viewport& viewport = AIngine::Application::GetViewport();
		glm::vec2 scale = glm::vec2((float)viewport.GetViewportWidth() / (float)TargetResolution.x, (float)viewport.GetViewportHeight() / (float)TargetResolution.y);
		return AIngine::Graphics::GetTextSize(Text, scale, m_fonts.at(FontSize));
	}
	Component * UIText::Copy(GameObject * const owner) const
	{
		UIText* copy = new UIText();

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

		//Text
		copy->Text = Text;
		copy->SetFont(m_currentFont);
		copy->AlignHorizontal = AlignHorizontal;
		copy->AlignVertical = AlignVertical;

		return copy;
	}

	void AlignTextHorizontally(float * x, float textWidth, const glm::vec2 & extents, TextAlignmentHorizontal align)
	{
		float center = 0;
		switch (align) {
		case TextAlignmentHorizontal::Left:
			*x = extents.x;
			break;
		case TextAlignmentHorizontal::Center:
			center = (extents.y + extents.x) * 0.5;
			*x = center - textWidth * 0.5f;
			break;
		case TextAlignmentHorizontal::Right:
			*x = extents.y - textWidth;
			break;
		}
	}
	void AlignTextVertically(float * y, float textHeight, const glm::vec2 & extents, TextAlignmentVertical align)
	{
		float center = 0;
		switch (align) {
		case TextAlignmentVertical::Top:
			*y = extents.x;
			break;
		case TextAlignmentVertical::Center:
			center = (extents.y + extents.x) * 0.5;
			*y = center - textHeight * 0.5f;
			break;
		case TextAlignmentVertical::Bottom:
			*y = extents.y - textHeight;
			break;
		}
	}
}