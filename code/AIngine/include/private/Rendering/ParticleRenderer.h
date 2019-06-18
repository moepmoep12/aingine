#pragma once
#include "Structures/Traverser.h"
#include "Rendering/shader.h"

#include <vector>
#include <glm/glm.hpp>

namespace AIngine {
	class ParticleEmitter;
}

namespace AIngine::Rendering {
	class ParticleRenderer : public Traverser {
	public:
		ParticleRenderer();
		virtual ~ParticleRenderer();

		// Inherited via Traverser
		virtual bool Traverse(GameObject * root) override;
		virtual bool Enter(GameObject & node) override;
		virtual bool Leave(GameObject & node) override;
		virtual bool Visit(GameObject & node) override;


		void DrawParticles(AIngine::ParticleEmitter& emitter);

	private:
		GLShaderProgram* m_shader;
		std::vector<glm::mat4> m_matrixStack;
		glm::mat4 m_modelMatrix = glm::mat4(1.0f);
		//unsigned int m_vao;
		float m_additiveRotation = 0.0f;
	};
}