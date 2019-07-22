#pragma once
#include "AIngine/Core.h"
#include "Effects.h"
#include "AIngine/UI/Button.h"
#include "AIngine/UI/Text.h"

namespace CrappyBird {
	class Player : public AIngine::Script {
	public:
		Player();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;
		virtual void OnGUI() override;
		virtual void PostInit() override;

		// Event is fired when the player enters the next screen
		AIngine::Events::Event<void, AIngine::Structures::RectangleF&> OnEnterNewScreen;

		AIngine::Events::Event<void> OnGameOverEvent;
		typedef AIngine::Events::EventHandler<void> OnGameOverEventHandler;

		AIngine::Events::Event<void> OnRestartGame;
		typedef AIngine::Events::EventHandler<void> OnRestartGameEventHandler;

		// returns the current size of the rocket
		glm::vec2 GetSize();

		// returns all effects that affect the player
		const std::vector<std::unique_ptr<Effect>>& GetActiveEffects() const { return m_activeEffects; }

		std::vector<glm::vec2> GetOriginalPhysVertices() const;

		/* returns the effect of the given type if in the activeEffect list*/
		template<class T>
		T* GetEffect() {
			auto it = m_activeEffects.begin();

			while (it != m_activeEffects.end())
			{
				if (typeid(T) == typeid(*it._Ptr->get())) {
					return dynamic_cast<T*>(it._Ptr->get());
				}
				it++;
			}

			return nullptr;
		}

		/* adds a effect */
		void AddEffect(std::unique_ptr<Effect> effect);

		// the index of the screen the player is currently in
		int CurrentScreenIndex = 1;

		bool IsGameOver = false;
;
		static const glm::vec4 s_finalFireColor1;
		static const glm::vec4 s_finalFireColor2;
		static const float s_lifeTime;

	protected:
		/* Callbacks */
		virtual void OnCollision(AIngine::Physics::Contact contact);
		virtual void OnSpawnFireParticles(Particle* particles, int count, const glm::vec2& pos);
		virtual void OnUpdateParticle(Particle& particle, float delta);
		virtual void OnSpawnCollisionParticle(Particle* particles, int count, const glm::vec2& pos);
		virtual void OnUpdateCollisionParticle(Particle& particle, float delta);
		virtual void OnGameOver();
		virtual void UpdateGameOverScreen(float delta);
		
		virtual void ResetGame();
		virtual void PlayEngineSound();

		// all effects currently affecting the player
		std::vector<std::unique_ptr<Effect>> m_activeEffects;

		std::vector<glm::vec2> m_originalVertices;

		// the physical body of the rocket
		AIngine::Physics::PhysicsComponent* m_physBody;
		float m_originalGameSpeed = 0;

		// the distance traveled aka Score
		float m_distanceTraveled = 0;

		float m_delta = 0;

		// the particle emitter which emitts fire
		ParticleEmitter* m_emitter;
		ParticleEmitter* m_collisionEmitter;

		const glm::vec2 m_spawnPosition = glm::vec2(1, 2.5);

		AIngine::UI::Button* retryButton = nullptr;
		AIngine::UI::UIText* retryText = nullptr;
		AIngine::UI::UIText* gameOverText = nullptr;
		AIngine::UI::Button::OnClickedEventHandler OnRetryClickedHandler;

		// EventHandler for the callbacks. Need to be kept in order to unsubscribe from events
		AIngine::ParticleEmitter::SpawnParticlesHandler OnSpawnFireParticlesHandler;
		AIngine::ParticleEmitter::SpawnParticlesHandler OnSpawnCollisionParticlesHandler;
		AIngine::ParticleEmitter::UpdateParticleHandler OnUpdateParticleHandler;
		AIngine::ParticleEmitter::UpdateParticleHandler OnUpdateCollisionParticleHandler;
		AIngine::Events::EventHandler<void, AIngine::Physics::Contact> OnCollisionEventHandler;
		OnGameOverEventHandler OnGameOverHandler;
	};
}