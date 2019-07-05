#include "Player.h"
#include "CrappyBird.h"
#include "Obstacle.h"
#include "Rendering/Viewport.h"

#include <random>

namespace CrappyBird {

	const float Player::s_standardImpulse = 0.075f;
	float Player::s_Impulse = Player::s_standardImpulse;

	// Constructor
	Player::Player()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
		m_originalGameSpeed = CrappyBird::s_GameSpeed;
		gameOverFont = Assets::Load<FontAsset>(AIngine::Assets::GetFontPath("AIngine/fonts/JOURNAL.ttf", 200));
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
		m_emitter = GetOwner()->GetComponent<ParticleEmitter>();
		m_physBody = GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>();
		m_physBody->SetFixedRotation(true);

		// create EventHandlers
		OnCollisionEventHandler = AIngine::Events::EventHandler<void, PhysicsComponent*>(std::bind(&Player::OnCollision, this, std::placeholders::_1));
		OnSpawnParticleHandler = AIngine::ParticleEmitter::SpawnParticleHandler(std::bind(&Player::OnSpawnParticle, this, std::placeholders::_1, std::placeholders::_2));
		OnUpdateParticleHandler = AIngine::ParticleEmitter::UpdateParticleHandler(std::bind(&Player::OnUpdateParticle, this, std::placeholders::_1));
		OnGameOverHandler = OnGameOverEventHandler(std::bind(&Player::OnGameOver, this));

		// register callbacks
		m_physBody->OnCollisionBegin += OnCollisionEventHandler;
		m_emitter->SpawnParticleEvent += OnSpawnParticleHandler;
		m_emitter->UpdateParticleEvent += OnUpdateParticleHandler;
		OnGameOverEvent += OnGameOverHandler;

		for (int i = 0; i < m_physBody->GetBodyInformation().verticesCount; i++) {
			m_originalVertices.push_back(m_physBody->GetBodyInformation().vertices[i]);
		}

		retryButton = AIngine::World::GetGameObject("RetryButton")->GetComponent<AIngine::UI::Button>();
		OnRetryClickedHandler = AIngine::UI::Button::OnClickedEventHandler(std::bind(&Player::ResetGame, this));
		retryButton->OnClickedEvent += OnRetryClickedHandler;
	}

	// End is called when gameplay ends for this script
	void Player::OnEnd()
	{
		// restore original gamespeed
		CrappyBird::s_GameSpeed = m_originalGameSpeed;

		// unsubscribe from events
		m_physBody->OnCollisionBegin -= OnCollisionEventHandler;
		m_emitter->SpawnParticleEvent -= OnSpawnParticleHandler;
		m_emitter->UpdateParticleEvent -= OnUpdateParticleHandler;
		OnGameOverEvent -= OnGameOverHandler;
		retryButton->OnClickedEvent -= OnRetryClickedHandler;

		m_activeEffects.clear();
	}

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

		// basic fire
		m_emitter->Update(delta, 20);

		if (AIngine::Input::IsMouseButtonPressed(0) || AIngine::Input::IsKeyPressed(AIngine::KeyCodes::SPACE)) {
			// accelerate
			m_emitter->Update(delta, 75);
			m_physBody->ApplyLinearImpulseToCenter(glm::vec2(0, -s_Impulse));
			PlayEngineSound();
		}

		// update score
		m_distanceTraveled += CrappyBird::s_GameSpeed * delta;

		// check if new screen has been entered and fire event
		if (m_distanceTraveled >= CurrentScreenIndex * 10) {
			CurrentScreenIndex++;
			glm::vec4 bounds = AIngine::World::GetBounds();
			AIngine::Structures::RectangleI nextScreen(bounds.y, 0, bounds.y, bounds.w / 2.0f);
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
	}

	// Callback for events
	void Player::OnEventData(AIngine::Events::EventData & e)
	{
		// key pressed
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {
			AIngine::Events::KeyPressedEvent::KeyPressedEventData pressedEvent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);

			if (pressedEvent.GetKeyCode() == AIngine::KeyCodes::SPACE) {
				OnGameOverEvent();
			}
		}
	}

	void Player::OnGUI()
	{
		// display the score
		std::stringstream ss;
		ss << "Score: " << (int)m_distanceTraveled;
		Graphics::Text(ss.str(), glm::vec2(10, 35), glm::vec2(2));
	}

	void Player::OnCollision(PhysicsComponent * other)
	{
		if (CrappyBird::s_DieOnCollision) {
			if (other->GetOwner()->GetName() == "Obstacle") {
				OnGameOverEvent();
			}
		}
	}

	static float s_lifeTime = 0.35f;

	void Player::OnSpawnParticle(Particle & particle, const glm::vec2 & pos)
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

	void Player::OnUpdateParticle(Particle & particle)
	{
		float dt = m_delta;
		particle.Position += particle.Velocity * dt;
		static glm::vec4 finalColor(1, 0, 0, 0);
		float t = AIngine::Math::CosErp(particle.Lifetime / s_lifeTime);
		float u = AIngine::Math::CosErp(particle.Lifetime / s_lifeTime);
		glm::vec4 a = glm::vec4(t, t, 0, u);
		particle.Color = glm::mix(finalColor, particle.Color, a); // value = 1 => firstColor
		particle.Rotation += particle.AngularVelocity * dt;
	}

	void Player::OnGameOver()
	{
		IsGameOver = true;
		m_physBody->SetEnabled(false);
		retryButton->GetOwner()->SetActive(true);

		static const std::vector<std::string> buttonTexts = { "Retry!", "One more time...", "This is unfair", "Ugh...", "Gimme cheats", "Again!" };

		const std::string chosenText = buttonTexts[AIngine::Util::Random::RandomInt(0, buttonTexts.size())];
		retryButton->Text = chosenText;

		static const float padding = retryButton->GetRectangle().width * 0.2f;
		glm::vec2 textSize = Graphics::GetTextSize(chosenText);

		float xScale = (retryButton->GetRectangle().width - padding) / textSize.x;
		static const float yScale = 0.75;
		float xsize = Graphics::GetTextSize(chosenText, glm::vec2(xScale, yScale)).x;
		retryButton->TextScale = glm::vec2(xScale, yScale);
	}

	void Player::UpdateGameOverScreen(float delta)
	{
		const AIngine::Rendering::Viewport& viewport = AIngine::Application::Get().GetViewport();
		//static const glm::vec2 scale = glm::vec2(3);
		static glm::vec2 textsize = AIngine::Graphics::GetTextSize("Game Over", glm::vec2(1), &gameOverFont->GetFont());
		float desiredSize = viewport.GetViewportWidth() * 0.75;

		float scale = desiredSize / textsize.x;
		glm::vec2 actualTextSize = AIngine::Graphics::GetTextSize("Game Over", glm::vec2(scale), &gameOverFont->GetFont());

		glm::vec2 center = viewport.GetCenter();
		glm::vec2 pos = center;
		pos.x -= actualTextSize.x * 0.5f;
		pos.y -= actualTextSize.y * 0.5f;
		glm::vec2 buttonPos = center;
		buttonPos.y += actualTextSize.y * 0.75;
		buttonPos.x -= retryButton->GetRectangle().width * 0.5;
		retryButton->SetPosition(buttonPos);
		AIngine::Graphics::Text("Game Over", pos, glm::vec2(scale), glm::vec3(1, 0, 0), 1, &gameOverFont->GetFont());
	}

	void Player::ResetGame()
	{
		retryButton->GetOwner()->SetActive(false);

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
		static AIngine::Sound* sound = GetOwner()->GetComponent<SoundComponent>()->GetSound(0);
		if (!sound->IsPlaying())
			GetOwner()->GetComponent<SoundComponent>()->Play(0);
	}
}