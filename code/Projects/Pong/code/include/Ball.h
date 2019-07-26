#pragma once
#include "AIngine/Core.h"
#include "Player.h"
#include "UI/Text.h"
#include "Experiment.h"

namespace Pong {
	class Ball : public AIngine::Script {
	public:
		Ball();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void OnWidget() override;

		SoundComponent* m_sound;
		PhysicsComponent* m_physBody;
		ParticleEmitter* m_emitter;
		AIngine::ParticleEmitter::SpawnParticlesHandler OnSpawnParticlesHandler;
		AIngine::ParticleEmitter::UpdateParticleHandler OnUpdateParticleHandler;
		AIngine::Events::EventHandler<void, AIngine::Physics::Contact> OnCollisionEventHandler;

		virtual void OnSpawnParticles(Particle* particles, int count, const glm::vec2& pos);
		virtual void OnUpdateParticle(Particle& particle, float delta);
		virtual void OnCollision(AIngine::Physics::Contact contact);

		float s_particleLifeTime = 0.06f;
		int spawnCount = 200;
		glm::vec4 finalColor = glm::vec4(0);
		glm::vec2 particleSize = glm::vec2(0.06);
		glm::vec4 startColor = glm::vec4(1.0, 0.66, 0.1, 1.0);
		Experiment* m_experiment;

	};
}