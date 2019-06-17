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
		glm::vec2(-0.2,-0.09),
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
		m_physBody = m_owner->GetComponent<AIngine::Physics::PhysicsComponent>();
		m_collisionHandler = AIngine::Events::EventHandler<void, PhysicsComponent*>(std::bind(&Player::OnCollision, this, std::placeholders::_1));
		GetOwner()->GetComponent<PhysicsComponent>()->OnCollisionBegin += m_collisionHandler;

		m_emitter = GetOwner()->GetComponent<ParticleEmitter>();
		m_emitter->SetAmount(10000);
		OnSpawnParticleHandler = AIngine::ParticleEmitter::SpawnParticleHandler(std::bind(&Player::SpawnParticle, this, std::placeholders::_1, std::placeholders::_2));
		OnUpdateParticleHandler = AIngine::ParticleEmitter::UpdateParticleHandler(std::bind(&Player::UpdateParticle, this, std::placeholders::_1));
		m_emitter->SpawnParticleEvent += OnSpawnParticleHandler;
		m_emitter->UpdateParticleEvent += OnUpdateParticleHandler;
		//m_emitter->SetLocalSpawnPosition(glm::vec2(-0.13, 0.05));
		//m_emitter->SetTexture(AIngine::Rendering::Texture2D(AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>("assets/Intellgine/textures/White.png")->GetBitmap()));
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

	static float s_lifeTime = 0.5f;

	void Player::SpawnParticle(Particle & particle, const glm::vec2 & pos)
	{
		static std::default_random_engine generator;
		static std::normal_distribution<double> distribution(2.0, 1.0);
		static std::normal_distribution<double> veldistribution(1.0, 0.5);

		float value = std::clamp(distribution(generator), 0.0, 3.0);
		glm::vec2 p = AIngine::Math::catmull_rom_spline(m_splinePoints, value);


		particle.Lifetime = s_lifeTime;
		particle.Velocity = glm::vec2(-0.3, 0.15) * CrappyBird::s_GameSpeed;
		particle.Velocity *= std::clamp(veldistribution(generator), 0.1, 2.0);
		particle.AngularVelocity = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0 - 0)));
		particle.Position = pos + p;
		particle.Color = glm::vec4(1, 1, 0, 1);
		particle.Size = glm::vec2(0.03);
	}

	void Player::UpdateParticle(Particle & particle)
	{
		float dt = AIngine::Application::Get().GetDeltaTime();
		particle.Position += particle.Velocity * dt;
		static glm::vec4 finalColor(3, 0, 0, 0);
		particle.Color = glm::mix(finalColor, particle.Color, particle.Lifetime / s_lifeTime); /*(finalColor - particle.Color) * particle.Lifetime + finalColor;*/
		particle.Rotation += particle.AngularVelocity * dt;
	}
}