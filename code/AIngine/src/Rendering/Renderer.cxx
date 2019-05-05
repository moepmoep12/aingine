#include "Rendering/Renderer.h"
#include "Application.h"
#include "Rendering/texture.h"

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

		Application& app = AIngine::Application::Get();
		GLfloat width = static_cast<GLfloat>(app.GetWindow().GetWidth());
		GLfloat height = static_cast<GLfloat>(app.GetWindow().GetHeight());

		glm::mat4 projection = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

		// configure shader
		m_shader->SetInteger("image", 0, true);
		m_shader->SetMatrix4("projection", projection);
	}

	void AIngine::Rendering::SpriteRenderer::Render(GameObject * root)
	{
		m_shader->Use();
		m_matrixStack.clear();
		//m_matrixStack.push_back(glm::mat4(1.0f));
		m_modelViewMatrix = glm::mat4(1.0f);

		root->Accept(*this);
	}

	SpriteRenderer::SpriteRenderer(GLShaderProgram* shader)
	{
		m_shader = shader;
		initRenderData();
	}

	SpriteRenderer::~SpriteRenderer()
	{
		glDeleteVertexArrays(1, &this->m_quadVAO);
	}

	bool SpriteRenderer::Enter(GameObject & node)
	{
		m_matrixStack.push_back(m_modelViewMatrix);
		//m_modelViewMatrix *= node.GetTransform();
		m_modelViewMatrix = glm::translate(m_modelViewMatrix, glm::vec3(node.GetLocalPosition(), 0.0f));
		m_modelViewMatrix = glm::rotate(m_modelViewMatrix, node.GetLocalRotation(), glm::vec3(0.0f, 0.0f, 1.0f));
		m_modelViewMatrix = glm::scale(m_modelViewMatrix, glm::vec3(node.GetLocalScale(), 1.0f));
		return true;

	}
	bool SpriteRenderer::Leave(GameObject & node)
	{
		m_modelViewMatrix = m_matrixStack.back();
		m_matrixStack.pop_back();
		return true;
	}
	bool SpriteRenderer::Visit(GameObject & node)
	{
		const Texture2D* textureComponent = node.GetComponent<Texture2D>();

		if (textureComponent) {
			m_matrixStack.push_back(m_modelViewMatrix);

			//m_modelViewMatrix *= node.GetTransform();
			m_modelViewMatrix = glm::translate(m_modelViewMatrix, glm::vec3(node.GetLocalPosition(), 0.0f));

			glm::vec2 textureSize = glm::vec2(node.GetLocalScale().x * textureComponent->Width, node.GetLocalScale().y * textureComponent->Height);

			m_modelViewMatrix = glm::translate(m_modelViewMatrix, glm::vec3(0.5f * textureSize.x, 0.5f * textureSize.y, 0.0f));
			m_modelViewMatrix = glm::rotate(m_modelViewMatrix, node.GetLocalRotation(), glm::vec3(0.0f, 0.0f, 1.0f));
			m_modelViewMatrix = glm::translate(m_modelViewMatrix, glm::vec3(-0.5f * textureSize.x, -0.5f * textureSize.y, 0.0f));

			m_modelViewMatrix = glm::scale(m_modelViewMatrix, glm::vec3(textureSize, 1.0f));

			m_shader->SetMatrix4("model", m_modelViewMatrix);
			m_shader->SetVector3f("spriteColor", textureComponent->GetColor());

			glActiveTexture(GL_TEXTURE0);
			textureComponent->Bind();

			glBindVertexArray(m_quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			m_modelViewMatrix = m_matrixStack.back();
			m_matrixStack.pop_back();

			return true;
		}
		else return false;
	}
}

