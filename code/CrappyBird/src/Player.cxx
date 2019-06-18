#include "Player.h"
#include "AIngine/Input.h"
#include "Events/Event.h"
#include "Events/InputEvents.h"
#include "CrappyBird.h"

#include <typeinfo>
#include <random>
#include <vector>

namespace CrappyBird {

	static std::vector<glm::vec2> m_splinePoints = {
		glm::vec2(-0.19,-0.08),
		glm::vec2(-0.15, -0.01),
		glm::vec2(-0.12, 0.1),
		glm::vec2(-0.13,0.2)
	};

	Player::Player()
	{
		SetName(typeid(*this).name());
		m_originalGameSpeed = CrappyBird::s_GameSpeed;
	}

	Player::~Player()
	{
	}

	glm::vec2 Player::GetSize()
	{
		return glm::vec2(1.4, 0.7);
	}

	void Player::OnStart()
	{
		GetOwner()->GetComponent<PhysicsComponent>()->SetFixedRotation(true);
		m_spawnPos = GetOwner()->GetLocalPosition();
		m_physBody = GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>();
		m_collisionHandler = AIngine::Events::EventHandler<void, PhysicsComponent*>(std::bind(&Player::OnCollision, this, std::placeholders::_1));
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin += m_collisionHandler;

		m_emitter = GetOwner()->GetComponent<ParticleEmitter>();
		OnSpawnParticleHandler = AIngine::ParticleEmitter::SpawnParticleHandler(std::bind(&Player::SpawnParticle, this, std::placeholders::_1, std::placeholders::_2));
		OnUpdateParticleHandler = AIngine::ParticleEmitter::UpdateParticleHandler(std::bind(&Player::UpdateParticle, this, std::placeholders::_1));
		m_emitter->SpawnParticleEvent += OnSpawnParticleHandler;
		m_emitter->UpdateParticleEvent += OnUpdateParticleHandler;
	}

	void Player::OnEnd()
	{
		CrappyBird::s_GameSpeed = m_originalGameSpeed;
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin -= m_collisionHandler;

		m_emitter->SpawnParticleEvent -= OnSpawnParticleHandler;
		m_emitter->UpdateParticleEvent -= OnUpdateParticleHandler;
	}

	void Player::OnEventData(AIngine::Events::EventData & e)
	{
		// key pressed
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {
			AIngine::Events::KeyPressedEvent::KeyPressedEventData pressedEvent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);
			if (pressedEvent.GetKeyCode() == AIngine::KeyCodes::R) {
			}
		}
	}

	void Player::OnGUI()
	{
		std::stringstream ss;
		ss << "Score: " << (int)m_distanceTraveled;
		Graphics::Text(ss.str(), glm::vec2(10, 20), glm::vec2(2));
	}

	void Player::Update(float deltatime)
	{
		// basic fire
		m_emitter->Update(AIngine::Application::Get().GetDeltaTime(), 20);

		if (AIngine::Input::IsMouseButtonPressed(0)) {
			// accelerate
			m_emitter->Update(AIngine::Application::Get().GetDeltaTime(), 75);
			m_physBody->ApplyLinearImpulseToCenter(glm::vec2(0, -0.075f));
		}

		m_distanceTraveled += CrappyBird::s_GameSpeed * deltatime;

		if (m_distanceTraveled >= CurrentScreenIndex * 10) {
			CurrentScreenIndex++;
			glm::vec4 bounds = AIngine::World::GetBounds();
			AIngine::Structures::RectangleI nextScreen(bounds.y, 0, bounds.y, bounds.w / 2.0f);
			OnEnterNewScreen(nextScreen);
		}

	}


	void Player::OnCollision(PhysicsComponent * other)
	{
		if (other->GetOwner()->GetName() == "Obstacle") {
			//GetOwner()->SetActive(false);
		}
	}

	static float s_lifeTime = 0.35f;

	void Player::SpawnParticle(Particle & particle, const glm::vec2 & pos)
	{
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

	void Player::UpdateParticle(Particle & particle)
	{
		float dt = AIngine::Application::Get().GetDeltaTime();
		particle.Position += particle.Velocity * dt;
		static glm::vec4 finalColor(1, 0, 0, 0);
		float t = AIngine::Math::CosErp(particle.Lifetime / s_lifeTime);
		float u = AIngine::Math::CosErp(particle.Lifetime / s_lifeTime);
		glm::vec4 a = glm::vec4(t, t, 0, u);
		particle.Color = glm::mix(finalColor, particle.Color, a); // value = 1 => firstColor
		particle.Rotation += particle.AngularVelocity * dt;
	}
}