#pragma once
#include "UI/UIELement.h"
#include "Rendering/Font.h"

#include <string>
#include <map>

namespace AIngine::UI {

	enum class TextAlignmentVertical {
		Top = 0,
		Center,
		Bottom
	};

	enum class TextAlignmentHorizontal {
		Left = 0,
		Center,
		Right
	};

	/* extents going from xmin to xmax, e.g. 100 - 400*/
	void AlignTextHorizontally(float* x, float textWidth, const glm::vec2& extents, TextAlignmentHorizontal align);

	/* extents going from ymax to ymin, e.g. 400-100 */
	void AlignTextVertically(float* y, float textHeight, const glm::vec2& extents, TextAlignmentVertical align);


	class UIText : public UIElement {
	public:
		UIText();
		virtual bool Render(AIngine::Rendering::GLShaderProgram& shader) const override;

		inline unsigned int GetFontSize() const { return FontSize; }
		void ChangeFontSize(unsigned int size);

		inline const AIngine::Rendering::Font& GetFont() const { return *m_currentFont; }
		void SetFont(AIngine::Rendering::Font* font);

		glm::vec2 GetTextSize() const;

		std::string Text;

		TextAlignmentVertical AlignVertical = TextAlignmentVertical::Top;
		TextAlignmentHorizontal AlignHorizontal = TextAlignmentHorizontal::Left;

	private:
		unsigned int FontSize = 50;
		AIngine::Rendering::Font* m_currentFont;
		std::map<unsigned int, AIngine::Rendering::Font*> m_fonts;

	protected:
		virtual Component* Copy(GameObject* const owner) const override;
	};
}