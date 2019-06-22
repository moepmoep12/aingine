#include "Rendering/ParticleRenderer.h"
#include "Assets/Assets.h"
#include "AIngine/ParticleEmitter.h"
#include "Rendering/Camera.h"

#include "Debug/log.h"

namespace AIngine::Rendering {

	ParticleRenderer::ParticleRenderer()
	{
		// Set up mesh and attribute properties
		//GLuint VBO;
		//GLfloat particle_quad[] = {
		//	// Pos      // Tex
		//	-0.5f, 0.5f, 0.0f, 1.0f,
		//	0.5f, -0.5f, 1.0f, 0.0f,
		//	-0.5f, -0.5f, 0.0f, 0.0f,

		//	-0.5f, 0.5f, 0.0f, 1.0f,
		//	0.5f, 0.5f, 1.0f, 1.0f,
		//	0.5f, -0.5f, 1.0f, 0.0f
		//};
		//glGenVertexArrays(1, &this->m_vao);
		//glGenBuffers(1, &VBO);
		//glBindVertexArray(this->m_vao);
		//// Fill mesh buffer
		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
		//// Set mesh attributes
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

		//glBindVertexArray(0);

		// load Shader
		std::string vs("AIngine/shader/screenshader/vertexParticle.glsl");
		std::string fs("AIngine/shader/screenshader/fragmentParticle.glsl");

		m_shader = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::ShaderAsset>(AIngine::Assets::ShaderPath(vs, fs))->GetShader();
	}

	ParticleRenderer::~ParticleRenderer()
	{
		//glDeleteVertexArrays(1, &this->m_vao);
	}

	bool ParticleRenderer::Traverse(GameObject * root)
	{
		m_shader->SetMatrix4(6 /*projection*/, Camera::Get().GetProjectionMatrix(), true);
		m_shader->SetMatrix4(7 /*viewmatrix*/, Camera::Get().GetViewMatrix());
		m_shader->SetInteger(8 /*image*/, 0);

		m_matrixStack.clear();
		m_additiveRotation = 0;
		m_modelMatrix = glm::mat4(1.0f);

		return root->Accept(*this);
	}

	bool ParticleRenderer::Enter(GameObject & node)
	{
		AIngine::ParticleEmitter* emitter = node.GetComponent<AIngine::ParticleEmitter>();

		if (emitter)
			DrawParticles(*emitter);

		m_matrixStack.push_back(m_modelMatrix);
		m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(node.GetLocalPosition(), 0.0f));
		m_additiveRotation += node.GetLocalRotation();

		return true;
	}

	bool ParticleRenderer::Leave(GameObject & node)
	{
		m_modelMatrix = m_matrixStack.back();
		m_matrixStack.pop_back();
		m_additiveRotation -= node.GetLocalRotation();
		return true;
	}

	bool ParticleRenderer::Visit(GameObject & node)
	{
		AIngine::ParticleEmitter* emitter = node.GetComponent<AIngine::ParticleEmitter>();
		if (emitter)
			DrawParticles(*emitter);

		return true;
	}

	void ParticleRenderer::DrawParticles(AIngine::ParticleEmitter & emitter)
	{

		glBlendFunc(GL_SRC_ALPHA, emitter.BlendFunc);

		GameObject& gameObject = *emitter.GetOwner();

		m_matrixStack.push_back(m_modelMatrix);

		// we position & rotate around the center
		m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(gameObject.GetLocalPosition(), 0.0f));
		m_modelMatrix = glm::rotate(m_modelMatrix, m_additiveRotation, glm::vec3(0.0f, 0.0f, 1.0f));
		//m_modelMatrix = glm::rotate(m_modelMatrix, gameObject.GetLocalRotation(), glm::vec3(0.0f, 0.0f, 1.0f));

		//m_matrixStack.push_back(m_modelMatrix);


		glBindVertexArray(emitter.m_vao);
		glActiveTexture(GL_TEXTURE0);
		emitter.GetTexture().Bind();

		m_shader->SetMatrix4(5 /*model*/, m_modelMatrix);


		std::vector<glm::vec2> offsets;
		std::vector<glm::vec2> sizes;
		std::vector<glm::vec4> colors;

		for (auto& particle : emitter.GetParticles()) {
			if (particle.Lifetime > 0.0f) {

				//m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(particle.Position, 0.0f));
				//m_modelMatrix = glm::rotate(m_modelMatrix, particle.Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
				//m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(particle.Size, 1));

				// configure shader
				//m_shader->SetMatrix4(3 /*model*/, m_modelMatrix);
				//m_shader->SetVector4f(5 /*spriteColor*/, particle.Color);

				sizes.push_back(particle.Size);
				colors.push_back(particle.Color);
				offsets.push_back(particle.Position);

				//m_modelMatrix = m_matrixStack.back();

				//glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}

		if (offsets.size() > 0) {
			glBindBuffer(GL_ARRAY_BUFFER, emitter.m_offsetVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * offsets.size(), &offsets[0]);

			glBindBuffer(GL_ARRAY_BUFFER, emitter.m_sizeVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * sizes.size(), &sizes[0]);

			glBindBuffer(GL_ARRAY_BUFFER, emitter.m_ColorVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * colors.size(), &colors[0]);

			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, offsets.size());
		}

		glBindVertexArray(0);
		glActiveTexture(0);

		//m_matrixStack.pop_back();
		m_modelMatrix = m_matrixStack.back();
		m_matrixStack.pop_back();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}