#include "Rendering/UIRenderer.h"
#include "Rendering/Camera.h"
#include "Assets/Assets.h"
#include "AIngine/GameObject.h"
#include "AIngine/World.h"
#include "UI/UIELement.h"

namespace AIngine::Rendering {

	GameObject* UIRenderer::canvas = nullptr;

	UIRenderer::UIRenderer()
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

		glm::mat4 projection = Camera::Get().GetProjectionMatrix();

		// load Shader
		std::string vs("AIngine/shader/screenshader/vertexScreen.glsl");
		std::string fs("AIngine/shader/screenshader/fragmentScreen.glsl");
		m_shader = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::ShaderAsset>(AIngine::Assets::ShaderPath(vs, fs))->GetShader();

		// configure shader
		m_shader->SetInteger(4, 0, true);
		m_shader->SetMatrix4(2, projection);
	}

	UIRenderer::~UIRenderer()
	{
		glDeleteVertexArrays(1, &this->m_quadVAO);
	}


	bool UIRenderer::Traverse(GameObject * root)
	{
		if (!canvas) return false;
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m_shader->SetMatrix4(2 /*projection*/, Camera::Get().GetProjectionMatrix(), true);
		m_shader->SetMatrix4(3 /*viewmatrix*/, glm::mat4(1));

		m_matrixStack.clear();
		m_additiveRotation = 0;
		m_modelMatrix = glm::mat4(1);

		return canvas->Accept(*this);
	}

	bool UIRenderer::Enter(GameObject & node)
	{
		auto components = node.GetComponents();
		AIngine::UI::UIElement* uielement = nullptr;

		for (auto comp : components) {
			uielement = dynamic_cast<AIngine::UI::UIElement*>(comp);
			if (uielement)
				break;
		}

		if (uielement)
		{
			if (uielement->IsActive()) {
				RenderUIElement(*uielement);
			}

			m_matrixStack.push_back(m_modelMatrix);
			m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(uielement->GetRectangle().GetCenter(), 0.0f));
			m_additiveRotation += node.GetLocalRotation();
			m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(node.GetLocalScale(), 1.0f));
		}


		return true;
	}

	bool UIRenderer::Leave(GameObject & node)
	{
		if (m_matrixStack.size() > 0) {
			m_modelMatrix = m_matrixStack.back();
			m_matrixStack.pop_back();
			m_additiveRotation -= node.GetLocalRotation();
		}
		return true;
	}

	bool UIRenderer::Visit(GameObject & node)
	{
		auto components = node.GetComponents();
		AIngine::UI::UIElement* uielement = nullptr;

		for (auto comp : components) {
			uielement = dynamic_cast<AIngine::UI::UIElement*>(comp);
			if (uielement)
				break;
		}
		if (uielement)
		{
			if (uielement->IsActive()) {
				RenderUIElement(*uielement);
			}
		}
		return true;
	}


	void UIRenderer::RenderUIElement(AIngine::UI::UIElement & element)
	{
		m_shader->Use();
		m_matrixStack.push_back(m_modelMatrix);

		const AIngine::Structures::Rectangle<int>& rect = element.GetRectangle();

		// we position & rotate around the center
		m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(rect.GetCenter(), 0.0f));
		m_modelMatrix = glm::rotate(m_modelMatrix, m_additiveRotation, glm::vec3(0.0f, 0.0f, 1.0f));
		m_modelMatrix = glm::rotate(m_modelMatrix, element.GetOwner()->GetLocalRotation(), glm::vec3(0.0f, 0.0f, 1.0f));
		m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(rect.width, rect.height, 1.0f));

		m_shader->SetMatrix4(1 /*model*/, m_modelMatrix);

		glActiveTexture(GL_TEXTURE0);
		element.Render(m_modelMatrix, *m_shader);
		glBindVertexArray(m_quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glActiveTexture(0);

		m_modelMatrix = m_matrixStack.back();
		m_matrixStack.pop_back();
	}
}