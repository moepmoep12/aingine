#pragma once 

#include "AIngine/Component.h"
#include "Rendering/texture.h"
#include <glm/glm.hpp>

namespace AIngine::Editor::Widget::Component {
	class SpriteComponentWidget;
}

namespace AIngine {
	class Sprite final : public Component {

		friend class AIngine::Editor::Widget::Component::SpriteComponentWidget;

	public:
		inline glm::vec2 GetLocalWorldSize()const { return m_localWorldSize; }
		inline void SetLocalWorldSize(const glm::vec2& size) { m_localWorldSize = size; }

		inline  glm::vec2 GetParallaxFactor() const { return m_parallaxFactor; }
		inline void SetParallaxFactor(const glm::vec2& factor) { m_parallaxFactor = factor; }

		inline  AIngine::Rendering::Texture2D&  GetTexture() { return m_texture; }
		inline void SetTexture(const AIngine::Rendering::Texture2D& texture) { m_texture = texture; SetName(texture.FileName); }

		inline glm::vec4 GetColor() const { return m_color; }
		inline void SetColor(const glm::vec4& color) { m_color = color;; }

		inline float GetAlpha() const { return m_color.w;; }
		inline void SetAlpha(GLfloat alpha) { m_color.w = alpha; }

		virtual ~Sprite();

	private:
		glm::vec4 m_color = glm::vec4(1);
		glm::vec2 m_localWorldSize = glm::vec2(0);
		glm::vec2 m_parallaxFactor = glm::vec2(1);
		AIngine::Rendering::Texture2D m_texture;

	protected:
		virtual Component* Copy(GameObject* const owner) const override;
	};
}