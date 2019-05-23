#pragma once
#include "Rendering/shader.h"
#include "Structures/Traverser.h"

#include <vector>
#include <glm/glm.hpp>

namespace AIngine::Rendering {

	class SpriteRenderer : public AIngine::Traverser {
	public:

		SpriteRenderer(AIngine::Rendering::GLShaderProgram* shader);
		virtual ~SpriteRenderer();

		// Inherited via Traverser
		virtual bool Traverse(GameObject* root) override;
		virtual bool Enter(GameObject & node) override;
		virtual bool Leave(GameObject & node) override;
		virtual bool Visit(GameObject & node) override;

		void initRenderData();

	private:
		std::vector<glm::mat4> m_matrixStack;
		glm::mat4 m_modelViewMatrix = glm::mat4(1.0f);
		GLShaderProgram* m_shader;
		GLuint m_quadVAO;

	};

}