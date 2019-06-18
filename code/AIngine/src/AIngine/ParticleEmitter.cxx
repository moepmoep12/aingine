#include "AIngine/ParticleEmitter.h"
#include "Application.h"

#include <random>
#include <time.h>

namespace AIngine {

	ParticleEmitter::ParticleEmitter()
	{
		srand(time(NULL));

		// Set up mesh and attribute properties
		GLuint VBO;
		GLfloat particle_quad[] = {
			// Pos      // Tex
			-0.5f, 0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.0f, 0.0f,

			-0.5f, 0.5f, 0.0f, 1.0f,
			0.5f, 0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, 1.0f, 0.0f
		};
		glGenVertexArrays(1, &this->m_vao);
		glGenBuffers(1, &VBO);
		glBindVertexArray(this->m_vao);
		// Fill mesh buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
		// Set mesh attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

		// setup offset buffer
		glGenBuffers(1, &m_offsetVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_offsetVBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * maxParticles, nullptr, GL_DYNAMIC_DRAW); // allocate space for max amount
		{
			GLuint location = 2;
			GLint components = 2;
			GLenum type = GL_FLOAT;
			GLboolean normalized = GL_FALSE;
			GLsizei datasize = sizeof(glm::vec2);
			char* pointer = 0;
			GLuint divisor = 1;
			glEnableVertexAttribArray(location); //location of each column
			glVertexAttribPointer(location, components, type, normalized, datasize, pointer); //tell other data
			glVertexAttribDivisor(location, divisor); //is it instanced?
		}

		// setup size buffer
		glGenBuffers(1, &m_sizeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_sizeVBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * maxParticles, nullptr, GL_DYNAMIC_DRAW); // allocate space for max amount
		{
			GLuint location = 3;
			GLint components = 2;
			GLenum type = GL_FLOAT;
			GLboolean normalized = GL_FALSE;
			GLsizei datasize = sizeof(glm::vec2);
			char* pointer = 0;
			GLuint divisor = 1;
			glEnableVertexAttribArray(location); //location of each column
			glVertexAttribPointer(location, components, type, normalized, datasize, pointer); //tell other data
			glVertexAttribDivisor(location, divisor); //is it instanced?
		}

		// setup rotation buffer
		glGenBuffers(1, &m_rotationVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_rotationVBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * maxParticles, nullptr, GL_DYNAMIC_DRAW); // allocate space for max amount
		{
			GLuint location = 4;
			GLint components = 1;
			GLenum type = GL_FLOAT;
			GLboolean normalized = GL_FALSE;
			GLsizei datasize = sizeof(GLfloat);
			char* pointer = 0;
			GLuint divisor = 1;
			glEnableVertexAttribArray(location); //location of each column
			glVertexAttribPointer(location, components, type, normalized, datasize, pointer); //tell other data
			glVertexAttribDivisor(location, divisor); //is it instanced?
		}

		// setup color buffer
		glGenBuffers(1, &m_ColorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * maxParticles, nullptr, GL_DYNAMIC_DRAW); // allocate space for max amount
		{
			GLuint location = 1;
			GLint components = 4;
			GLenum type = GL_FLOAT;
			GLboolean normalized = GL_FALSE;
			GLsizei datasize = sizeof(glm::vec4);
			char* pointer = 0;
			GLuint divisor = 1;
			glEnableVertexAttribArray(location); //location of each column
			glVertexAttribPointer(location, components, type, normalized, datasize, pointer); //tell other data
			glVertexAttribDivisor(location, divisor); //is it instanced?
		}

		glBindVertexArray(0);
	}

	ParticleEmitter::~ParticleEmitter()
	{
		glDeleteVertexArrays(1, &this->m_vao);
	}

	void ParticleEmitter::SetAmount(int value)
	{
		value = std::clamp(value, 1, (int)maxParticles);
		if (value > m_amount) {
			int diff = value - m_amount;

			for (int i = 0; i < diff; i++) {
				m_particles.push_back(Particle());
			}

			glBindBuffer(GL_ARRAY_BUFFER, m_offsetVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * value, nullptr, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, m_sizeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * value, nullptr, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, m_rotationVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * value, nullptr, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * value, nullptr, GL_DYNAMIC_DRAW);
		}
		else {
			int diff = m_amount - value;
			for (int i = 0; i < diff; i++) {
				m_particles.erase(m_particles.begin());
			}
		}

		m_amount = value;
	}

	void ParticleEmitter::OnUpdate(float deltatime)
	{
		if (!AIngine::Application::IsRunning()) return;

		for (int i = 0; i < m_amount; ++i)
		{
			Particle& p = this->m_particles[i];
			p.Lifetime -= deltatime; // reduce life
			if (p.Lifetime > 0.0f)
			{	// particle is alive, thus update
				UpdateParticleEvent(p);
			}
		}
	}

	void ParticleEmitter::Update(float delta, int particlesToSpawn)
	{
		for (int i = 0; i < particlesToSpawn; i++) {
			int availableParticle = GetAvailableParticle();
			SpawnParticleEvent(m_particles[availableParticle], m_localSpawnPosition);
		}
	}

	int s_lastParticleUsed = 0;

	int ParticleEmitter::GetAvailableParticle() const
	{
		// First search from last used particle, this will usually return almost instantly
		for (int i = s_lastParticleUsed; i < m_amount; ++i) {
			if (m_particles[i].Lifetime <= 0.0f) {
				s_lastParticleUsed = i;
				return i;
			}
		}
		// Otherwise, do a linear search
		for (GLuint i = 0; i < s_lastParticleUsed; ++i) {
			if (this->m_particles[i].Lifetime <= 0.0f) {
				s_lastParticleUsed = i;
				return i;
			}
		}
		// All particles are taken
		s_lastParticleUsed = 0;
		return 0;
	}
}

namespace AIngine::Events {

	// initialize EventHandler for OnSpawnParticle
	int AIngine::Events::EventHandler<void, AIngine::Particle&, const glm::vec2&>::counter = 0;
	// UpdateParticle
	int AIngine::Events::EventHandler<void, AIngine::Particle&>::counter = 0;
}