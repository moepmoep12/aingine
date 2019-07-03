#include "UI/Text.h"
#include "Rendering/shader.h"
#include "AIngine/Graphics.h"
#include "Assets/Assets.h"

namespace AIngine::UI {


	UIText::UIText()
	{
		// load default
		static const std::string defaultFontPath = "AIngine/fonts/arial.ttf";
		std::string path = AIngine::Assets::GetFontPath(defaultFontPath, FontSize);
		m_currentFont = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::FontAsset>(path)->GetFont();
		m_fonts[FontSize] = m_currentFont;
	}

	bool UIText::Render(const glm::mat4 & modelMatrix, AIngine::Rendering::GLShaderProgram & shader) const
	{
		AIngine::Graphics::TextInstant(Text, GetRectangleAbsolute().GetPosition(), glm::vec2(1), TintColor, TintColor.w, m_fonts.at(FontSize));
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
		}

		FontSize = size;
	}

	void UIText::SetFont(AIngine::Rendering::Font * font)
	{
		if (font != m_currentFont) {
			m_fonts.clear();
			m_fonts[font->Size] = font;
			m_currentFont = font;
		}
	}
}