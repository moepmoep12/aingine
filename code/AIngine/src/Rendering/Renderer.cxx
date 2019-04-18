#include "Rendering/Renderer.h"

namespace AIngine::Rendering {

	void AIngine::Rendering::SpriteRenderer::initRenderData()
	{
		// Configure VAO/VBO
		GLuint VBO;
		GLfloat vertices[] = {
			// Pos      // Tex
			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f
		};

		glGenVertexArrays(1, &this->m_quadVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(this->m_quadVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void AIngine::Rendering::SpriteRenderer::Render(SceneNode * root)
	{
		m_shader->Use();
		m_matrixStack.clear();
		m_matrixStack.push_back(glm::mat4(1.0f));
		m_modelViewMatrix = glm::mat4(1.0f);

		root->Accept(*this);
	}

	SpriteRenderer::SpriteRenderer(GLShaderProgram* shader)
	{
		initRenderData();
		m_shader = shader;
	}

	SpriteRenderer::~SpriteRenderer()
	{
		glDeleteVertexArrays(1, &this->m_quadVAO);
	}

	bool SpriteRenderer::Enter(GroupNode & node)
	{
		m_matrixStack.push_back(m_modelViewMatrix);
		m_modelViewMatrix *= node.GetTransform();
		return true;
	}
	bool SpriteRenderer::Leave(GroupNode & node)
	{
		m_modelViewMatrix = m_matrixStack.back();
		m_matrixStack.pop_back();
		return true;
	}
	//bool SpriteRenderer::Visit(SceneNode & node)
	//{
	//	/******************************* TO DO *************************/
	//	return false;
	//}
	bool SpriteRenderer::Visit(ShapeNode & node)
	{
		m_matrixStack.push_back(m_modelViewMatrix);

		m_modelViewMatrix *= node.GetTransform();
		m_modelViewMatrix = glm::translate(m_modelViewMatrix, glm::vec3(node.GetPosition(), 0.0f));

		m_modelViewMatrix = glm::translate(m_modelViewMatrix, glm::vec3(0.5f * node.GetSize().x, 0.5f * node.GetSize().y, 0.0f));
		m_modelViewMatrix = glm::rotate(m_modelViewMatrix, node.GetRotation(), glm::vec3(0.0f, 0.0f, 1.0f));
		m_modelViewMatrix = glm::translate(m_modelViewMatrix, glm::vec3(-0.5f * node.GetSize().x, -0.5f * node.GetSize().y, 0.0f));

		m_modelViewMatrix = glm::scale(m_modelViewMatrix, glm::vec3(node.GetScale(), 1.0f));

		m_shader->SetMatrix4("m_modelViewMatrix", m_modelViewMatrix);


		glActiveTexture(GL_TEXTURE0);
		node.GetTexture().Bind();

		glBindVertexArray(m_quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		m_modelViewMatrix = m_matrixStack.back();
		m_matrixStack.pop_back();

		return true;
	}
}

