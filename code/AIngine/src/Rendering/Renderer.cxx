#include "Rendering/Renderer.h"
#include "Rendering/Camera.h"
#include "Rendering/Viewport.h"
#include "Rendering/texture.h"
#include "AIngine/Sprite.h"
#include "Editor/Editor.h"
#include "Assets/Assets.h"
#include "Application.h"
#include "AIngine/Constants.h"

#include <algorithm>

namespace AIngine::Rendering {

	bool SpriteRenderer::AnimateOutline = false;

	void AIngine::Rendering::SpriteRenderer::initRenderData()
	{
		// Configure VAO/VBO
		GLuint VBO;
		GLfloat vertices[] = {
			// Pos      // Tex
			-0.5f, 0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.0f, 0.0f,

			-0.5f, 0.5f, 0.0f, 1.0f,
			0.5f, 0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, 1.0f, 0.0f
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

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_ZERO, GL_REPLACE, GL_REPLACE);

		glm::mat4 projection = Camera::Get().GetProjectionMatrix();

		// configure shader
		m_shader->SetInteger(4, 0, true);
		m_shader->SetMatrix4(2, projection);

		// load outlineShader
		std::string vs("assets/Intellgine/shader/screenshader/vertexScreen.glsl");
		std::string fs("assets/Intellgine/shader/debug/fragmentOutline.glsl");

		std::string path;
		path.append(vs).append(";").append(fs);
		m_outlineShader = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::ShaderAsset>(path)->GetShader();
		m_outlineShader->SetInteger(0, 0, true);

		SetViewport();

	}

	void SpriteRenderer::SetViewport()
	{
		const Camera& cam = Camera::Get();
		const Viewport& viewport = cam.GetViewport();
		GLfloat windowHeight = static_cast<GLfloat>(viewport.GetWindowHeight());
		glm::vec2 bottomLeft = glm::vec2(viewport.GetTopLeftCornerPosition().x, viewport.GetTopLeftCornerPosition().y + viewport.GetViewportHeight());

		glViewport((GLint)bottomLeft.x, (GLint)(viewport.GetWindowHeight() - bottomLeft.y), (GLsizei)viewport.GetViewportWidth(), (GLsizei)viewport.GetViewportHeight());
	}

	void SpriteRenderer::RenderSprite(AIngine::Sprite& sprite, GLShaderProgram* shader)
	{
		shader->Use();

		GameObject& gameObject = *sprite.GetOwner();
		glm::vec2 textureSize = sprite.GetLocalWorldSize();
		textureSize.x *= gameObject.GetLocalScale().x;
		textureSize.y *= gameObject.GetLocalScale().y;

		m_matrixStack.push_back(m_modelMatrix);
		shader->SetMatrix4(3 /*viewmatrix*/, Camera::Get().GetViewMatrix(sprite.GetParallaxFactor()));

		// we position & rotate around the center
		m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(gameObject.GetLocalPosition(), 0.0f));
		m_modelMatrix = glm::rotate(m_modelMatrix, m_additiveRotation, glm::vec3(0.0f, 0.0f, 1.0f));
		m_modelMatrix = glm::rotate(m_modelMatrix, gameObject.GetLocalRotation(), glm::vec3(0.0f, 0.0f, 1.0f));
		m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(textureSize, 1.0f));

		// configure shader
		shader->SetMatrix4(1 /*model*/, m_modelMatrix);
		shader->SetVector4f(5 /*spriteColor*/, sprite.GetColor());

		// draw
		glActiveTexture(GL_TEXTURE0);
		sprite.GetTexture().Bind();

		glBindVertexArray(m_quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glActiveTexture(0);

		m_modelMatrix = m_matrixStack.back();
		m_matrixStack.pop_back();
	}

	static float s_outlineTime = 0.0f;

	void SpriteRenderer::RenderOutline(AIngine::Sprite& sprite)
	{
		glEnable(GL_STENCIL_TEST);

		// 1st. render pass, draw objects as normal, writing to the stencil buffer
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF); // no mask
		RenderSprite(sprite, m_shader);

		// 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
		// Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
		// the objects' size differences, making it look like borders.
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // if we have a stencil value != 1 
		glStencilMask(0x00); // then this v

		const float borderSreenSize = 15.0f;
		glm::vec2 p1 = Camera::Get().ScreenToWorldPoint(glm::vec2(0));
		glm::vec2 p2 = Camera::Get().ScreenToWorldPoint(glm::vec2(borderSreenSize, 0));
		float borderWorldSize = p2.x - p1.x;
		glm::vec2 originalTexSize = sprite.GetLocalWorldSize();


		sprite.SetLocalWorldSize(glm::vec2(originalTexSize.x + borderWorldSize, originalTexSize.y + borderWorldSize));
		RenderSprite(sprite, m_outlineShader);
		sprite.SetLocalWorldSize(originalTexSize);

		glStencilMask(0xFF); // no mask
		glClear(GL_STENCIL_BUFFER_BIT);
		glDisable(GL_STENCIL_TEST);
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
		m_shader->SetMatrix4(2 /*projection*/, Camera::Get().GetProjectionMatrix(), true);
		m_outlineShader->SetMatrix4(2 /*projection*/, Camera::Get().GetProjectionMatrix(), true);

		m_matrixStack.clear();
		m_additiveRotation = 0;
		m_modelMatrix = glm::mat4(1.0f);

		if (AnimateOutline) {
			s_outlineTime = std::fmod(s_outlineTime + 3 * Application::Get().GetDeltaTime(), 2 * M_PI);
			m_outlineShader->SetFloat("time", s_outlineTime);
		}
		else {
			m_outlineShader->SetFloat("time", 3.55f);
		}
		
		return root->Accept(*this);

	}

	bool SpriteRenderer::Enter(GameObject & node)
	{
		AIngine::Sprite* spriteComponent = node.GetComponent<AIngine::Sprite>();

		if (spriteComponent) {
			if (spriteComponent->IsActive()) {
				RenderSprite(*spriteComponent, m_shader);
			}
		}

		m_matrixStack.push_back(m_modelMatrix);
		m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(node.GetLocalPosition(), 0.0f));
		m_additiveRotation += node.GetLocalRotation();
		m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(node.GetLocalScale(), 1.0f));
		return true;

	}
	bool SpriteRenderer::Leave(GameObject & node)
	{
		m_modelMatrix = m_matrixStack.back();
		m_matrixStack.pop_back();
		m_additiveRotation -= node.GetLocalRotation();
		return true;
	}
	bool SpriteRenderer::Visit(GameObject & node)
	{
		AIngine::Sprite* spriteComponent = node.GetComponent<AIngine::Sprite>();

		if (spriteComponent && spriteComponent->IsActive()) {
			const std::vector<GameObject*>* selectedObjects = AIngine::Editor::Editor::GetSelectedObjects();
			if (selectedObjects) {
				if (std::find(selectedObjects->begin(), selectedObjects->end(), &node) != selectedObjects->end()) {
					RenderOutline(*spriteComponent); // draw outline
				}
				else {
					RenderSprite(*spriteComponent, m_shader);
				}
			}
			else {
				RenderSprite(*spriteComponent, m_shader);
			}
			return true;
		}
		else return false;
	}
}

