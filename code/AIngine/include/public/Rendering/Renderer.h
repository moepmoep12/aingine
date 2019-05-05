#pragma once
#include "shader.h"
#include <vector>
#include <glm/glm.hpp>
#include "Traverser.h"

namespace AIngine::Rendering {

	class SpriteRenderer : public AIngine::Traverser {
	public:

		SpriteRenderer(GLShaderProgram* shader);
		virtual ~SpriteRenderer();

		// Inherited via Traverser
		virtual bool Traverse(GameObject* root) override;

		virtual bool Enter(GameObject & node) override;
		virtual bool Leave(GameObject & node) override;
		virtual bool Visit(GameObject & node) override;

	private:
		std::vector<glm::mat4> m_matrixStack;
		glm::mat4 m_modelViewMatrix = glm::mat4(1.0f);
		GLShaderProgram* m_shader;
		GLuint m_quadVAO;

		void initRenderData();
	};

}