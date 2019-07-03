#pragma once
#include "UI/UIELement.h"
#include "Rendering/Font.h"

#include <string>
#include <map>

namespace AIngine::UI {
	class UIText : public UIElement {
	public:
		UIText();
		virtual bool Render(const glm::mat4& modelMatrix, AIngine::Rendering::GLShaderProgram& shader) const override;

		inline unsigned int GetFontSize() const { return FontSize; }
		void ChangeFontSize(unsigned int size);

		inline const AIngine::Rendering::Font& GetFont() const { return *m_currentFont; }
		void SetFont(AIngine::Rendering::Font* font);

		std::string Text;
	private:
		unsigned int FontSize = 50;
		AIngine::Rendering::Font* m_currentFont;
		std::map<unsigned int, AIngine::Rendering::Font*> m_fonts;
	};
}