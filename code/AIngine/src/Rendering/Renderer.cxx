#include "Rendering/Renderer.h"
#include "Rendering/Camera.h"
#include "Rendering/Viewport.h"
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

		// generate buffers
		glGenVertexArrays(1, &this->m_quadVAO);
		glGenBuffers(1, &VBO);

		// fill buffer data
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// configure buffer ptr
		glBindVertexArray(this->m_quadVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glm::mat4 projection = Camera::Get().GetProjectionMatrix();

		// configure shader
		m_shader->SetInteger("image", 0, true);
		m_shader->SetMatrix4("projection", projection);

		SetViewport();
	}

	void SpriteRenderer::SetViewport()
	{
		const Camera& cam = Camera::Get();
		const Viewport& viewport = cam.GetViewport();
		GLfloat windowHeight = static_cast<GLfloat>(viewport.GetWindowHeight());
		glm::vec2 bottomLeft = glm::vec2(viewport.GetTopLeftCornerPosition().x, viewport.GetTopLeftCornerPosition().y + viewport.GetViewportHeight());

		glViewport((GLint)bottomLeft.x, (GLint)viewport.GetWindowHeight() - bottomLeft.y, (GLsizei)viewport.GetViewportWidth(), (GLsizei)viewport.GetViewportHeight());
	}

	void SpriteRenderer::RenderSprite(Texture2D & texture)
	{
		GameObject& gameObject = *texture.GetOwner();
		glm::vec2 textureSize = texture.GetLocalWorldSize();
		textureSize.x *= gameObject.GetLocalScale().x;
		textureSize.y *= gameObject.GetLocalScale().y;

		m_matrixStack.push_back(m_modelMatrix);
		m_shader->SetMatrix4("view", Camera::Get().GetViewMatrix(texture.GetParallaxFactor()), true);

		// we position & rotate around the center
		m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(gameObject.GetLocalPosition(), 0.0f));
		m_modelMatrix = glm::rotate(m_modelMatrix, gameObject.GetLocalRotation(), glm::vec3(0.0f, 0.0f, 1.0f));
		m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(-0.5f * textureSize.x, -0.5f * textureSize.y, 0.0f));
		m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(textureSize, 1.0f));

		// save the model matrix 
		texture.SetModelMatrix(m_modelMatrix);

		// configure shader
		m_shader->SetMatrix4("model", m_modelMatrix);
		m_shader->SetVector3f("spriteColor", texture.GetColor());

		// draw
		glActiveTexture(GL_TEXTURE0);
		texture.Bind();

		glBindVertexArray(m_quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glActiveTexture(0);

		m_modelMatrix = m_matrixStack.back();
		m_matrixStack.pop_back();
	}

	SpriteRenderer::SpriteRenderer(AIngine::Rendering::GLShaderProgram* shader)
	{
		m_shader = shader;
	}

	SpriteRenderer::~SpriteRenderer()
	{
		glDeleteVertexArrays(1, &this->m_quadVAO);
	}

	bool SpriteRenderer::Traverse(GameObject * root)
	{
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// configure shader
		m_shader->Use();
		m_shader->SetMatrix4("view", Camera::Get().GetViewMatrix());
		m_shader->SetMatrix4("projection", Camera::Get().GetProjectionMatrix());

		m_matrixStack.clear();
		m_modelMatrix = glm::mat4(1.0f);


		return root->Accept(*this);

	}

	bool SpriteRenderer::Enter(GameObject & node)
	{
		Texture2D* textureComponent = node.GetComponent<Texture2D>();

		if (textureComponent) {
			if (textureComponent->IsActive()) {
				RenderSprite(*textureComponent);
			}
		}

		m_matrixStack.push_back(m_modelMatrix);
		m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(node.GetLocalPosition(), 0.0f));
		m_modelMatrix = glm::rotate(m_modelMatrix, node.GetLocalRotation(), glm::vec3(0.0f, 0.0f, 1.0f));
		m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(node.GetLocalScale(), 1.0f));
		return true;

	}
	bool SpriteRenderer::Leave(GameObject & node)
	{
		m_modelMatrix = m_matrixStack.back();
		m_matrixStack.pop_back();
		return true;
	}
	bool SpriteRenderer::Visit(GameObject & node)
	{
		Texture2D* textureComponent = node.GetComponent<Texture2D>();

		if (textureComponent && textureComponent->IsActive()) {

			RenderSprite(*textureComponent);
			return true;
		}
		else return false;
	}
}

