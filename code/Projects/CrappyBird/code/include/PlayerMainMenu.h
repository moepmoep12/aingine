#pragma once
#include "AIngine/Core.h"

namespace CrappyBird {
	class PlayerMainMenu : public AIngine::Script {
	public:
		PlayerMainMenu();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

	private:
		/* Callbacks */
		void OnSpawnParticle(Particle& particle, const glm::vec2& pos);
		void OnUpdateParticle(Particle& particle);

	private:
		// the particle emitter which emitts fire
		ParticleEmitter* m_emitter;

		float m_delta = 0;

		// EventHandler for the callbacks. Need to be kept in order to unsubscribe from events
		AIngine::ParticleEmitter::SpawnParticleHandler OnSpawnParticleHandler;
		AIngine::ParticleEmitter::UpdateParticleHandler OnUpdateParticleHandler;
	};
}