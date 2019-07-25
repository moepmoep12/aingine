#include "Player.h"
#include "CrappyBird.h"
#include "Obstacle.h"
#include "Rendering/Viewport.h"
#include "UI/Image.h"

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
		if (effect->Start(this)) {
			m_activeEffects.push_back(std::move(effect));
		}
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
		OnCollisionEventHandler = AIngine::Events::EventHandler<void, AIngine::Physics::Contact>(BIND_FN_1(Player::OnCollision));
		OnCollisionEndEventHandler = AIngine::Events::EventHandler<void, AIngine::Physics::Contact>(BIND_FN_1(Player::OnCollisionEnd));
		OnSpawnFireParticlesHandler = AIngine::ParticleEmitter::SpawnParticlesHandler(BIND_FN_3(Player::OnSpawnFireParticles));
		OnSpawnCollisionParticlesHandler = AIngine::ParticleEmitter::SpawnParticlesHandler(BIND_FN_3(Player::OnSpawnCollisionParticle));
		OnUpdateParticleHandler = AIngine::ParticleEmitter::UpdateParticleHandler(BIND_FN_2(Player::OnUpdateParticle));
		OnUpdateCollisionParticleHandler = AIngine::ParticleEmitter::UpdateParticleHandler(BIND_FN_2(Player::OnUpdateCollisionParticle));
		OnGameOverHandler = OnGameOverEventHandler(BIND_FN_0(Player::OnGameOver));

		// register callbacks
		m_physBody->OnCollisionBegin += OnCollisionEventHandler;
		m_physBody->OnCollisionEnd += OnCollisionEndEventHandler;
		m_emitter->SpawnParticlesEvent += OnSpawnFireParticlesHandler;
		m_emitter->UpdateParticleEvent += OnUpdateParticleHandler;
		m_collisionEmitter->SpawnParticlesEvent += OnSpawnCollisionParticlesHandler;
		m_collisionEmitter->UpdateParticleEvent += OnUpdateCollisionParticleHandler;


		OnGameOverEvent += OnGameOverHandler;

		for (int i = 0; i < m_physBody->GetBodyInformation().verticesCount; i++) {
			m_originalVertices.push_back(m_physBody->GetBodyInformation().vertices[i]);
		}

		retryButton = AIngine::World::GetGameObject("RetryButton")->GetComponent<AIngine::UI::Button>();
		menuButton = AIngine::World::GetGameObject("BackButton")->GetComponent<AIngine::UI::Button>();
		menuButton->OnClickedEvent += []() {CrappyBird::LoadScene(0); };
		OnRetryClickedHandler = AIngine::UI::Button::OnClickedEventHandler(BIND_FN_0(Player::ResetGame));
		retryButton->OnClickedEvent += OnRetryClickedHandler;
		retryText = retryButton->GetComponent<AIngine::UI::UIText>();
		gameOverText = AIngine::World::GetGameObject("GameOverText")->GetComponent<AIngine::UI::UIText>();

		m_effectDisplays.push_back(AIngine::World::GetGameObject("SpeedEffect")->GetComponent<EffectDurationDisplay>());
		m_effectDisplays.push_back(AIngine::World::GetGameObject("SlowEffect")->GetComponent<EffectDurationDisplay>());
		m_effectDisplays.push_back(AIngine::World::GetGameObject("ShrinkEffect")->GetComponent<EffectDurationDisplay>());

#ifndef EDITOR
		CrappyBird::Get().GetWindow().SetMouseVisible(false);
#endif
	}

	// End is called when gameplay ends for this script
	void Player::OnEnd()
	{
		// restore original gamespeed
		CrappyBird::s_GameSpeed = m_originalGameSpeed;

		// unsubscribe from events
		m_physBody->OnCollisionBegin -= OnCollisionEventHandler;
		m_physBody->OnCollisionEnd -= OnCollisionEndEventHandler;
		m_emitter->SpawnParticlesEvent -= OnSpawnFireParticlesHandler;
		m_emitter->UpdateParticleEvent -= OnUpdateParticleHandler;
		m_collisionEmitter->SpawnParticlesEvent -= OnSpawnCollisionParticlesHandler;
		m_collisionEmitter->UpdateParticleEvent -= OnUpdateCollisionParticleHandler;
		OnGameOverEvent -= OnGameOverHandler;
		retryButton->OnClickedEvent -= OnRetryClickedHandler;
		menuButton->OnClickedEvent = AIngine::Events::Event<void>();
		m_activeEffects.clear();
		m_effectDisplays.clear();
	}

	static float velMultiplier = 1.5f;

	// Update is called once per frame
	void Player::Update(float delta)
	{
		m_delta = delta;

		if (IsGameOver) {
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

		PositionEffectDisplays();

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

		std::vector<Effect*> effects;
		for (int i = 0; i < m_activeEffects.size(); i++) {
			Effect* effect = m_activeEffects[i].get();
			effects.push_back(effect);
		}

		std::sort(effects.begin(), effects.end(), [](const Effect* a, const Effect* b) -> bool {return(a->Duration - a->Age) < (b->Duration - b->Age); });

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

	void Player::OnCollisionEnd(AIngine::Physics::Contact contact)
	{
		if (!IsGameOver)
			m_physBody->SetEnabled(true);
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
		//normal *= -1;
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
#ifndef EDITOR
		CrappyBird::Get().GetWindow().SetMouseVisible(true);
#endif
		IsGameOver = true;
		m_physBody->SetEnabled(false);
		retryButton->GetOwner()->SetActive(true);
		menuButton->GetOwner()->SetActive(true);
		gameOverText->GetOwner()->SetActive(true);

		static const std::vector<std::string> buttonTexts = { "Retry!", "One more time...", "This is unfair", "Ugh...", "Gimme cheats", "Again!" };

		const std::string chosenText = buttonTexts[AIngine::Util::Random::RandomInt(0, buttonTexts.size() - 1)];
		retryText->Text = chosenText;
		float xsize = Graphics::GetTextSize(chosenText, glm::vec2(1), &retryText->GetFont()).x;
		float rectWidth = (float)retryButton->GetRectangle().width;
		float scale = (rectWidth* 0.9f) / xsize;
		retryText->ChangeFontSize(std::roundl(scale * retryText->GetFontSize()));
	}

	void Player::ResetGame()
	{
#ifndef EDITOR
		CrappyBird::Get().GetWindow().SetMouseVisible(false);
#endif
		retryButton->GetOwner()->SetActive(false);
		gameOverText->GetOwner()->SetActive(false);
		menuButton->GetOwner()->SetActive(false);
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

		// end active Effects
		auto& it = m_activeEffects.begin();
		while (it != m_activeEffects.end()) {
			std::unique_ptr<Effect>* effect = it._Ptr;
			effect->get()->Undo();
			it = m_activeEffects.erase(it);
		}


		OnRestartGame();
	}

	void Player::PlayEngineSound()
	{
		AIngine::Sound* sound = GetOwner()->GetComponent<SoundComponent>()->GetSound(0);
		if (!sound->IsPlaying())
			GetOwner()->GetComponent<SoundComponent>()->Play(0);
	}

	void Player::PositionEffectDisplays()
	{
		std::vector<EffectDurationDisplay*> activeDisplays;
		for (auto display : m_effectDisplays)
			if (display->IsActive())
				activeDisplays.push_back(display);

		std::sort(activeDisplays.begin(), activeDisplays.end(), [](const EffectDurationDisplay* a, const EffectDurationDisplay* b)-> bool {return a->Remaining < b->Remaining; });

		static const int height = 35;
		int ypos = 0;

		for (auto display : activeDisplays) {
			display->SetYPos(ypos);
			ypos += height;
		}
	}
}

int AIngine::Events::EventHandler<void, AIngine::Structures::RectangleF&>::counter = 0;