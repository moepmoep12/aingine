#include "AIngine/ParticleEmitter.h"

#include <random>
#include <time.h>

namespace AIngine {

	ParticleEmitter::ParticleEmitter()
	{
		srand(time(NULL));
	}

	void ParticleEmitter::SetAmount(int value)
	{
		if (value > m_amount) {
			int diff = value - m_amount;

			for (int i = 0; i < diff; i++) {
				m_particles.push_back(Particle());
			}
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