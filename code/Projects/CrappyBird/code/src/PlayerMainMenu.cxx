#include "PlayerMainMenu.h"
#include "CrappyBird.h"
#include "Player.h"

#include <random>
#include <imgui.h>

namespace CrappyBird {

	// Constructor
	PlayerMainMenu::PlayerMainMenu()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void PlayerMainMenu::OnStart()
	{
		m_emitter = GetOwner()->GetComponent<ParticleEmitter>();
		OnSpawnParticleHandler = AIngine::ParticleEmitter::SpawnParticlesHandler(BIND_FN_3(PlayerMainMenu::OnSpawnParticles));
		OnUpdateParticleHandler = AIngine::ParticleEmitter::UpdateParticleHandler(BIND_FN_1(PlayerMainMenu::OnUpdateParticle));

		// register callbacks
		m_emitter->SpawnParticlesEvent += OnSpawnParticleHandler;
		m_emitter->UpdateParticleEvent += OnUpdateParticleHandler;
	}

	// End is called when gameplay ends for this script
	void PlayerMainMenu::OnEnd()
	{
		// unsubscribe from events
		m_emitter->SpawnParticlesEvent -= OnSpawnParticleHandler;
		m_emitter->UpdateParticleEvent -= OnUpdateParticleHandler;
	}

	// Update is called once per frame
	void PlayerMainMenu::Update(float delta)
	{
		m_delta = delta;

		static const float duration = 0.25;
		static float currentDuration = 0;
		static const float startHeight = GetOwner()->GetLocalPosition().y;
		const float maxHeight = startHeight - CrappyBird::s_Impulse;
		const float distance = startHeight - maxHeight;
		static float direction = -1; // -1 = upwards

		currentDuration += delta;
		if (currentDuration >= duration) {
			currentDuration = 0;
			direction *= -1;
		}
		float t = AIngine::Math::CosErp(currentDuration / duration);
		float heightDest = direction == -1 ? startHeight - t * distance : maxHeight + t * distance;

		GetOwner()->SetLocalPosition(glm::vec2(GetOwner()->GetLocalPosition().x, heightDest));

		// basic fire
		m_emitter->Update(delta, 100);
	}

	// Callback for events
	void PlayerMainMenu::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void PlayerMainMenu::OnSpawnParticles(Particle*  particles, int count, const glm::vec2 & pos)
	{
		std::vector<glm::vec2> m_splinePoints = {
		   glm::vec2(-0.19,-0.08) * GetOwner()->GetLocalScale(),
		   glm::vec2(-0.15, -0.01)  * GetOwner()->GetLocalScale(),
		   glm::vec2(-0.12, 0.1)  * GetOwner()->GetLocalScale(),
		   glm::vec2(-0.13,0.2)  * GetOwner()->GetLocalScale()
		};
		static std::default_random_engine generator;
		static std::normal_distribution<double> distribution(1.5, 1.0);
		static std::normal_distribution<double> veldistribution(1.0, 0.5);
		static std::normal_distribution<double> coldistribution(1.0, 0.2);
		static auto colFunc = [](float val) {
			return (2.0 / 9.0) * val * val + (-2.0 / 3.0) * val + (1.0);
		};

		for (int i = 0; i < count; i++) {
			Particle& particle = particles[i];
			float value = std::clamp(distribution(generator), 0.0, 3.0);
			glm::vec2 p = AIngine::Math::catmull_rom_spline(m_splinePoints, value);


			particle.Lifetime = Player::s_lifeTime;

			particle.Velocity = glm::vec2(-0.6, 0.3) * 1.5f * (CrappyBird::s_GameSpeed / CrappyBird::s_originalGameSpeed);
			// randomize velocity a little
			particle.Velocity *= std::clamp(veldistribution(generator), 0.1, 2.0);

			particle.AngularVelocity = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0 - 0)));

			particle.Position = pos + p;

			static const glm::vec4 Color1(1.0 * coldistribution(generator), colFunc(value), 0, 1);
			static const glm::vec4 Color2(0.01, 0, 1 * coldistribution(generator), 1);
			particle.Color = (CrappyBird::s_levelIndex == 1 || CrappyBird::s_levelIndex == 3) ? Color1 : Color2;

			particle.Size = glm::vec2(0.03) * (float)coldistribution(generator);
		}
	}

	void PlayerMainMenu::OnUpdateParticle(Particle & particle)
	{
		float dt = m_delta;
		particle.Position += particle.Velocity * dt;
		static const glm::vec4 finalColor1(1, 0, 0, 0);
		float t = AIngine::Math::CosErp(particle.Lifetime / Player::s_lifeTime);
		float u = AIngine::Math::SinErp(particle.Lifetime / Player::s_lifeTime);
		glm::vec4 a = glm::vec4(t, t, t, u);
		glm::vec4 finalColor = (CrappyBird::s_levelIndex == 1 || CrappyBird::s_levelIndex == 3) ? Player::s_finalFireColor1 : Player::s_finalFireColor2;
		particle.Color = glm::mix(finalColor, particle.Color, a); // value = 1 => firstColor
		particle.Rotation += particle.AngularVelocity * dt;
	}
}