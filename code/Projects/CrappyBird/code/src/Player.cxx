#include "Player.h"
#include "CrappyBird.h"
#include "Obstacle.h"
#include "Rendering/Viewport.h"

#include <random>

namespace CrappyBird {

	const float Player::s_lifeTime = 0.15f;
	const glm::vec4 Player::s_finalFireColor1(1, 0, 0, 0);
	const glm::vec4 Player::s_finalFireColor2(0.47, 0.11, 1, 0);

	// Constructor
	Player::Player()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
		m_originalGameSpeed = CrappyBird::s_GameSpeed;
	}

	glm::vec2 Player::GetSize()
	{
		return glm::vec2(1.4, 0.7);
	}

	std::vector<glm::vec2> Player::GetOriginalPhysVertices() const
	{
		return m_originalVertices;
	}

	void Player::AddEffect(std::unique_ptr<Effect>  effect)
	{
		if (effect->Start(this))
			m_activeEffects.push_back(std::move(effect));
	}

	// Start is called when gameplay starts for this script
	void Player::OnStart()
	{
		m_scoreText = AIngine::World::GetGameObject("ScoreText")->GetComponent<AIngine::UI::UIText>();
		m_emitter = GetOwner()->GetComponent<ParticleEmitter>();
		m_collisionEmitter = GetOwner()->GetChildren()[0]->GetComponent<ParticleEmitter>();
		m_physBody = GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>();
		m_physBody->SetEnabled(true);
		m_physBody->SetFixedRotation(true);

		// create EventHandlers
		OnCollisionEventHandler = AIngine::Events::EventHandler<void, AIngine::Physics::Contact>(std::bind(&Player::OnCollision, this, std::placeholders::_1));
		OnSpawnFireParticlesHandler = AIngine::ParticleEmitter::SpawnParticlesHandler(std::bind(&Player::OnSpawnFireParticles, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		OnSpawnCollisionParticlesHandler = AIngine::ParticleEmitter::SpawnParticlesHandler(std::bind(&Player::OnSpawnCollisionParticle, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		OnUpdateParticleHandler = AIngine::ParticleEmitter::UpdateParticleHandler(std::bind(&Player::OnUpdateParticle, this, std::placeholders::_1, std::placeholders::_2));
		OnUpdateCollisionParticleHandler = AIngine::ParticleEmitter::UpdateParticleHandler(std::bind(&Player::OnUpdateCollisionParticle, this, std::placeholders::_1, std::placeholders::_2));
		OnGameOverHandler = OnGameOverEventHandler(std::bind(&Player::OnGameOver, this));

		// register callbacks
		m_physBody->OnCollisionBegin += OnCollisionEventHandler;
		m_emitter->SpawnParticlesEvent += OnSpawnFireParticlesHandler;
		m_emitter->UpdateParticleEvent += OnUpdateParticleHandler;
		m_collisionEmitter->SpawnParticlesEvent += OnSpawnCollisionParticlesHandler;
		m_collisionEmitter->UpdateParticleEvent += OnUpdateCollisionParticleHandler;


		OnGameOverEvent += OnGameOverHandler;

		for (int i = 0; i < m_physBody->GetBodyInformation().verticesCount; i++) {
			m_originalVertices.push_back(m_physBody->GetBodyInformation().vertices[i]);
		}

		retryButton = AIngine::World::GetGameObject("RetryButton")->GetComponent<AIngine::UI::Button>();
		OnRetryClickedHandler = AIngine::UI::Button::OnClickedEventHandler(std::bind(&Player::ResetGame, this));
		retryButton->OnClickedEvent += OnRetryClickedHandler;
		retryText = retryButton->GetComponent<AIngine::UI::UIText>();
		gameOverText = AIngine::World::GetGameObject("GameOverText")->GetComponent<AIngine::UI::UIText>();
	}

	// End is called when gameplay ends for this script
	void Player::OnEnd()
	{
		// restore original gamespeed
		CrappyBird::s_GameSpeed = m_originalGameSpeed;

		// unsubscribe from events
		m_physBody->OnCollisionBegin -= OnCollisionEventHandler;
		m_emitter->SpawnParticlesEvent -= OnSpawnFireParticlesHandler;
		m_emitter->UpdateParticleEvent -= OnUpdateParticleHandler;
		m_collisionEmitter->SpawnParticlesEvent -= OnSpawnCollisionParticlesHandler;
		m_collisionEmitter->UpdateParticleEvent -= OnUpdateCollisionParticleHandler;
		OnGameOverEvent -= OnGameOverHandler;
		retryButton->OnClickedEvent -= OnRetryClickedHandler;

		m_activeEffects.clear();
	}

	static float velMultiplier = 1.5f;

	// Update is called once per frame
	void Player::Update(float delta)
	{
		m_delta = delta;

		if (IsGameOver) {
			UpdateGameOverScreen(delta);
			return;
		}
		// Check if GameOver
		if (GetOwner()->GetWorldPosition().x < 0) {
			OnGameOverEvent();
		}



		if (AIngine::Input::IsMouseButtonPressed(AIngine::MouseButton::BUTTON_LEFT) || AIngine::Input::IsKeyPressed(AIngine::KeyCode::SPACE)) {
			// accelerate
			velMultiplier = CrappyBird::s_GameSpeed;
			m_emitter->Update(delta, 100);
			m_physBody->ApplyLinearImpulseToCenter(glm::vec2(0, -CrappyBird::s_Impulse));
			PlayEngineSound();
		}
		else if (AIngine::Input::IsMouseButtonPressed(AIngine::MouseButton::BUTTON_RIGHT))
		{
			m_physBody->ApplyLinearImpulseToCenter(glm::vec2(0, CrappyBird::s_Impulse * 0.15f));
		}
		else {
			velMultiplier = 1.5f * CrappyBird::s_GameSpeed / m_originalGameSpeed;
			m_emitter->Update(delta, 50);
		}

		// update score
		m_distanceTraveled += CrappyBird::s_GameSpeed * delta;

		// check if new screen has been entered and fire event
		if (m_distanceTraveled >= CurrentScreenIndex * 10) {
			CurrentScreenIndex++;
			glm::vec4 bounds = AIngine::World::GetBounds();
			AIngine::Structures::RectangleF nextScreen(bounds.y, 0, bounds.y, bounds.w / 2.0f);
			OnEnterNewScreen(nextScreen);
		}

		// update Effects
		auto& it = m_activeEffects.begin();
		while (it != m_activeEffects.end()) {
			std::unique_ptr<Effect>* effect = it._Ptr;
			effect->get()->Age += delta;

			if (effect->get()->Age >= effect->get()->Duration) {
				effect->get()->End();
				it = m_activeEffects.erase(it);
			}
			else {
				effect->get()->Update(delta);
				it++;
			}
		}

		// Create Collision Particles
		if (m_physBody->GetContact()) {
			m_collisionEmitter->Update(delta, 10 * m_physBody->GetVelocity().length());
		}
	}

	// Callback for events
	void Player::OnEventData(AIngine::Events::EventData & e)
	{
		// key pressed
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {
			AIngine::Events::KeyPressedEvent::KeyPressedEventData pressedEvent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);

			if (pressedEvent.GetKeyCode() == AIngine::KeyCode::ESCAPE) {
				CrappyBird::LoadScene(0);
			}
		}
	}

	void Player::OnGUI()
	{
		// display the score
		std::stringstream ss;
		ss << "Score : " << (int)m_distanceTraveled;
		m_scoreText->Text = ss.str();
		//Graphics::Text(ss.str(), glm::vec2(10, 35), glm::vec2(2));
	}

	void Player::PostInit()
	{
		GetOwner()->SetActive(!CrappyBird::s_AgentLearning);
	}

	void Player::OnCollision(AIngine::Physics::Contact contact)
	{
		if (CrappyBird::s_DieOnCollision) {
			if (contact.Other->GetOwner()->GetName() == "Obstacle") {
				OnGameOverEvent();
			}
		}
	}


	void Player::OnSpawnFireParticles(Particle* particles, int count, const glm::vec2 & pos)
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


			particle.Lifetime = s_lifeTime;

			particle.Velocity = glm::vec2(-0.6, 0.3) * velMultiplier;
			// randomize velocity a little
			particle.Velocity *= std::clamp(veldistribution(generator), 0.1, 2.0);

			particle.AngularVelocity = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0 - 0)));

			particle.Position = pos + p;

			// color is more red in the middle
			static const glm::vec4 Color1(1.0 * coldistribution(generator), colFunc(value), 0, 1);
			static const glm::vec4 Color2(0.01, 0, 1 * coldistribution(generator), 1);
			particle.Color = (CrappyBird::s_levelIndex == 1 || CrappyBird::s_levelIndex == 3) ? Color1 : Color2;

			particle.Size = glm::vec2(0.03) * (float)coldistribution(generator);
		}
	}

	static const float s_collisionParticleLifeTime = 0.05f;

	void Player::OnSpawnCollisionParticle(Particle* particles, int count, const glm::vec2 & pos)
	{
		static std::default_random_engine generator;
		static std::normal_distribution<double> lifeTimeDistribution(1.0, 0.3);
		static std::normal_distribution<double> velocityDistribution(1.0, 0.5);
		static const glm::vec2 particleSize(0.025);
		// orange
		static const glm::vec4 startColor(1.0, 0.66, 0.1, 1.0);

		glm::vec2 origin = m_physBody->GetContact()->ContactPoints[0].WorldPoint - GetOwner()->GetWorldPosition();
		/*	DEBUG_INFO("{0}  {1}", origin.x, origin.y);*/
		glm::vec2 normal = m_physBody->GetContact()->Normal;
		normal *= -1;
		const glm::vec2 perpendicular(-normal.y, normal.x);
		float rotationStep = M_PI / count;
		for (int i = 0; i < count; i++) {
			Particle& particle = particles[i];
			particle.Lifetime = s_collisionParticleLifeTime * lifeTimeDistribution(generator);
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


	void Player::OnUpdateParticle(Particle & particle, float delta)
	{
		float dt = delta;
		particle.Position += particle.Velocity * dt;
		float t = AIngine::Math::CosErp(particle.Lifetime / s_lifeTime);
		float u = AIngine::Math::SinErp(particle.Lifetime / s_lifeTime);
		glm::vec4 a = glm::vec4(t, t, 0, u);
		glm::vec4 finalColor = (CrappyBird::s_levelIndex == 1 || CrappyBird::s_levelIndex == 3) ? s_finalFireColor1 : s_finalFireColor2;
		particle.Color = glm::mix(finalColor, particle.Color, a); // value = 1 => firstColor
		particle.Rotation += particle.AngularVelocity * dt;
	}

	void Player::OnUpdateCollisionParticle(Particle & particle, float delta)
	{
		static const glm::vec4 finalColor(0.984, 1.0, 0.1, 1);
		float dt = delta;
		particle.Position += particle.Velocity * dt;
		particle.Rotation += particle.AngularVelocity * dt;
		particle.Color = glm::mix(finalColor, particle.Color, AIngine::Math::CosErp(particle.Lifetime / s_lifeTime));
	}

	void Player::OnGameOver()
	{
		IsGameOver = true;
		m_physBody->SetEnabled(false);
		retryButton->GetOwner()->SetActive(true);
		gameOverText->GetOwner()->SetActive(true);

		static const std::vector<std::string> buttonTexts = { "Retry!", "One more time...", "This is unfair", "Ugh...", "Gimme cheats", "Again!" };

		const std::string chosenText = buttonTexts[AIngine::Util::Random::RandomInt(0, buttonTexts.size() - 1)];
		retryText->Text = chosenText;
		float xsize = Graphics::GetTextSize(chosenText, glm::vec2(1), &retryText->GetFont()).x;
		float scale = ((float)retryButton->GetRectangleNative().width * 0.9f) / xsize;
		retryText->ChangeFontSize(std::roundl(scale * retryText->GetFontSize()));

		//static const float padding = retryButton->GetRectangle().width * 0.2f;
		//glm::vec2 textSize = Graphics::GetTextSize(chosenText);

		//float xScale = (retryButton->GetRectangle().width - padding) / textSize.x;
		//static const float yScale = 0.75;
		//float xsize = Graphics::GetTextSize(chosenText, glm::vec2(xScale, yScale)).x;
		//retryButton->TextScale = glm::vec2(xScale, yScale);

		//for (auto& effect : m_activeEffects)
		//	effect->End();

		//m_activeEffects.clear();
	}

	void Player::UpdateGameOverScreen(float delta)
	{
		//const AIngine::Rendering::Viewport& viewport = AIngine::Application::Get().GetViewport();
		////static const glm::vec2 scale = glm::vec2(3);
		//static glm::vec2 textsize = AIngine::Graphics::GetTextSize("Game Over", glm::vec2(1), &gameOverFont->GetFont());
		//float desiredSize = viewport.GetViewportWidth() * 0.75;

		//float scale = desiredSize / textsize.x;
		//glm::vec2 actualTextSize = AIngine::Graphics::GetTextSize("Game Over", glm::vec2(scale), &gameOverFont->GetFont());

		//glm::vec2 center = viewport.GetCenter();
		//glm::vec2 pos = center;
		//pos.x -= actualTextSize.x * 0.5f;
		//pos.y -= actualTextSize.y * 0.5f;
		////glm::vec2 buttonPos = center;
		////buttonPos.y += actualTextSize.y * 0.75;
		////buttonPos.x -= retryButton->GetRectangle().width * 0.5;
		////retryButton->SetPosition(buttonPos);
		//AIngine::Graphics::Text("Game Over", pos, glm::vec2(scale), glm::vec3(1, 0, 0), 1, &gameOverFont->GetFont());
	}

	void Player::ResetGame()
	{
		retryButton->GetOwner()->SetActive(false);
		gameOverText->GetOwner()->SetActive(false);
		m_distanceTraveled = 0;
		IsGameOver = false;
		m_physBody->SetEnabled(true);
		GetOwner()->SetWorldPosition(m_spawnPosition);
		CurrentScreenIndex = 1;

		GameObject* obstacleParent = AIngine::World::GetGameObject("Obstacles");
		for (auto& child : obstacleParent->GetChildren()) {
			child->SetWorldPosition(glm::vec2(-5, 0));
			child->SetActive(false);
		}
		GameObject* pickupParent = AIngine::World::GetGameObject("PickUpFactory");
		for (auto& child : pickupParent->GetChildren()) {
			child->SetActive(false);
		}

		OnRestartGame();

	}

	void Player::PlayEngineSound()
	{
		AIngine::Sound* sound = GetOwner()->GetComponent<SoundComponent>()->GetSound(0);
		if (!sound->IsPlaying())
			GetOwner()->GetComponent<SoundComponent>()->Play(0);
	}
}

int AIngine::Events::EventHandler<void, AIngine::Structures::RectangleF&>::counter = 0;