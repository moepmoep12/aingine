#include "Ball.h"
#include "Pong.h"

#include <random>
#include <imgui.h>

namespace Pong {

	// Constructor
	Ball::Ball()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void Ball::OnStart()
	{
		m_emitter = GetComponent<ParticleEmitter>();
		m_physBody = GetComponent<PhysicsComponent>();
		//m_sound =  GetComponent<SoundComponent>();

		OnSpawnParticlesHandler = AIngine::ParticleEmitter::SpawnParticlesHandler(std::bind(&Ball::OnSpawnParticles, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		OnUpdateParticleHandler = AIngine::ParticleEmitter::UpdateParticleHandler(std::bind(&Ball::OnUpdateParticle, this, std::placeholders::_1, std::placeholders::_2));
		OnCollisionEventHandler = AIngine::Events::EventHandler<void, AIngine::Physics::Contact>(std::bind(&Ball::OnCollision, this, std::placeholders::_1));

		m_emitter->SpawnParticlesEvent += OnSpawnParticlesHandler;
		m_emitter->UpdateParticleEvent += OnUpdateParticleHandler;
		m_physBody->OnCollisionBegin += OnCollisionEventHandler;
	}

	// End is called when gameplay ends for this script
	void Ball::OnEnd()
	{
		m_emitter->SpawnParticlesEvent -= OnSpawnParticlesHandler;
		m_emitter->UpdateParticleEvent -= OnUpdateParticleHandler;
		m_physBody->OnCollisionBegin -= OnCollisionEventHandler;
	}

	void Ball::OnWidget()
	{
		ImGui::DragInt("Spawn Count", &spawnCount);
		ImGui::DragFloat("LifeTime", &s_particleLifeTime);
		float size = particleSize.x;
		if (ImGui::DragFloat("ParticleSize", &size)) {
			particleSize = glm::vec2(size);
		}
		float* startcolor[] = { &startColor.x, &startColor.y,&startColor.z, &startColor.w };
		ImGui::ColorEdit4("StartColor", *startcolor);
		float* finalcolor[] = { &finalColor.x, &finalColor.y,&finalColor.z, &finalColor.w };
		ImGui::ColorEdit4("FinalColor", *finalcolor);
	}


	void Ball::OnSpawnParticles(Particle * particles, int count, const glm::vec2 & pos)
	{
		static std::default_random_engine generator;
		static std::normal_distribution<double> lifeTimeDistribution(1.0, 0.3);
		static std::normal_distribution<double> velocityDistribution(1.0, 0.5);

		const AIngine::Physics::Contact* contact = GetComponent<PhysicsComponent>()->GetContact();
		glm::vec2 origin = contact->ContactPoints[0].WorldPoint - GetOwner()->GetWorldPosition();
		glm::vec2 normal = contact->Normal;

		const glm::vec2 perpendicular(-normal.y, normal.x);
		float rotationStep = M_PI / count;
		for (int i = 0; i < count; i++) {
			Particle& particle = particles[i];
			particle.Lifetime = s_particleLifeTime * lifeTimeDistribution(generator);
			glm::mat4 mat(1);
			mat = glm::rotate(mat, -rotationStep * i, glm::vec3(0, 0, 1));
			glm::vec4 vel = glm::vec4(perpendicular.x, perpendicular.y, 0, 1) * mat;
			particle.Velocity = vel;
			particle.Velocity *= (double)m_physBody->GetVelocity().length() * std::clamp(velocityDistribution(generator), 0.1, 2.0);
			particle.AngularVelocity = 0;
			particle.Position = pos + origin;
			particle.Size = particleSize;
			particle.Size *= velocityDistribution(generator);
			particle.Color = startColor;
		}
	}

	void Ball::OnUpdateParticle(Particle & particle, float delta)
	{
		particle.Position += particle.Velocity * delta;
		float t = AIngine::Math::CosErp(particle.Lifetime / s_particleLifeTime);
		float u = AIngine::Math::SinErp(particle.Lifetime / s_particleLifeTime);
		glm::vec4 a = glm::vec4(t, t, 0, u);
		particle.Color = glm::mix(finalColor, particle.Color, a); // value = 1 => firstColor
		particle.Rotation += particle.AngularVelocity * delta;
	}

	void Ball::OnCollision(AIngine::Physics::Contact contact)
	{
		m_emitter->Update(Pong::Get().GetDeltaTime(), spawnCount);
		//m_sound->Play(AIngine::Util::Random::RandomInt(0, m_sound->GetSounds().size() - 1));
	}
}