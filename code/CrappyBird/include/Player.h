#pragma once
#include "AIngine/Core.h"
#include "Effects.h"

namespace CrappyBird {
	class Player : public AIngine::Script
	{
	public:

		Player();
		virtual ~Player();

		// inherited via Script
		virtual void OnStart() override;
		virtual void Update(float deltatime) override;
		virtual void OnEnd() override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;
		virtual void OnGUI() override;

		// Event is fired when the player enters the next screen
		AIngine::Events::Event<void, AIngine::Structures::RectangleI&> OnEnterNewScreen;

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

	private:
		/* Callbacks */
		void OnCollision(PhysicsComponent* other);
		void OnSpawnParticle(Particle& particle, const glm::vec2& pos);
		void OnUpdateParticle(Particle& particle);

		// all effects currently affecting the player
		std::vector<std::unique_ptr<Effect>> m_activeEffects;

		std::vector<glm::vec2> m_originalVertices;

		// the physical body of the rocket
		AIngine::Physics::PhysicsComponent* m_physBody;
		float m_originalGameSpeed = 0;

		// the distance traveled aka Score
		float m_distanceTraveled = 0;

		// the particle emitter which emitts fire
		ParticleEmitter* m_emitter;

		// EventHandler for the callbacks. Need to be kept in order to unsubscribe from events
		AIngine::ParticleEmitter::SpawnParticleHandler OnSpawnParticleHandler;
		AIngine::ParticleEmitter::UpdateParticleHandler OnUpdateParticleHandler;
		AIngine::Events::EventHandler<void, PhysicsComponent*> OnCollisionEventHandler;
	};
}