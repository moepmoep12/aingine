#pragma once
#include "Structures/Traverser.h"
#include "Rendering/shader.h"

#include <vector>
#include <glm/glm.hpp>

namespace AIngine::UI {
	class UIElement;
}

namespace AIngine::Rendering {

	class Texture2D;

	class UIRenderer : public AIngine::Traverser {
	public:
		UIRenderer();
		virtual ~UIRenderer();
		// Inherited via Traverser
		virtual bool Traverse(GameObject* root) override;
		virtual bool Enter(GameObject & node) override;
		virtual bool Leave(GameObject & node) override;
		virtual bool Visit(GameObject & node) override;

		static GameObject* canvas;

	private:
		void RenderUIElement(AIngine::UI::UIElement& element);

	private:
		GLShaderProgram* m_shader;
		GLuint m_quadVAO;
		float m_additiveRotation = 0.0f;
		//std::vector<glm::mat4> m_matrixStack;
		glm::mat4 m_modelMatrix = glm::mat4(1.0f);

	};
}