#include "PlayerMainMenu.h"
#include "CrappyBird.h"

#include <random>

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
		OnSpawnParticleHandler = AIngine::ParticleEmitter::SpawnParticleHandler(std::bind(&PlayerMainMenu::OnSpawnParticle, this, std::placeholders::_1, std::placeholders::_2));
		OnUpdateParticleHandler = AIngine::ParticleEmitter::UpdateParticleHandler(std::bind(&PlayerMainMenu::OnUpdateParticle, this, std::placeholders::_1));

		// register callbacks
		m_emitter->SpawnParticleEvent += OnSpawnParticleHandler;
		m_emitter->UpdateParticleEvent += OnUpdateParticleHandler;
	}

	// End is called when gameplay ends for this script
	void PlayerMainMenu::OnEnd()
	{
		// unsubscribe from events
		m_emitter->SpawnParticleEvent -= OnSpawnParticleHandler;
		m_emitter->UpdateParticleEvent -= OnUpdateParticleHandler;
	}

	// Update is called once per frame
	void PlayerMainMenu::Update(float delta)
	{
		m_delta = delta;

		// basic fire
		m_emitter->Update(AIngine::Application::Get().GetDeltaTime(), 100);
	}

	// Callback for events
	void PlayerMainMenu::OnEventData(AIngine::Events::EventData & e)
	{
	}

	static float s_lifeTime = 0.15f;

	void PlayerMainMenu::OnSpawnParticle(Particle & particle, const glm::vec2 & pos)
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

		float value = std::clamp(distribution(generator), 0.0, 3.0);
		glm::vec2 p = AIngine::Math::catmull_rom_spline(m_splinePoints, value);


		particle.Lifetime = s_lifeTime;

		particle.Velocity = glm::vec2(-0.6, 0.3) * CrappyBird::s_GameSpeed;
		// randomize velocity a little
		particle.Velocity *= std::clamp(veldistribution(generator), 0.1, 2.0);

		particle.AngularVelocity = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0 - 0)));

		particle.Position = pos + p;

		// color is more red in the middle
		particle.Color = glm::vec4(1.0 * coldistribution(generator), colFunc(value), 0, 1);

		particle.Size = glm::vec2(0.03) * (float)coldistribution(generator);
	}

	void PlayerMainMenu::OnUpdateParticle(Particle & particle)
	{
		float dt = m_delta;
		particle.Position += particle.Velocity * dt;
		static glm::vec4 finalColor(1, 0, 0, 0);
		float t = AIngine::Math::CosErp(particle.Lifetime / s_lifeTime);
		float u = AIngine::Math::SinErp(particle.Lifetime / s_lifeTime);
		glm::vec4 a = glm::vec4(t, t, 0, u);
		particle.Color = glm::mix(finalColor, particle.Color, a); // value = 1 => firstColor
		particle.Rotation += particle.AngularVelocity * dt;
	}
}