#include "CrappyBird.h"
#include "Events/InputEvents.h"
#include <random>
#include <time.h>

AIngine::Application* AIngine::CreateApplication() {
	return new CrappyBird::CrappyBird();
}

namespace CrappyBird {

	static float s_cloudOffset = 0.0f;
	static float s_cloudRepeatWidth;
	static glm::vec2 VisibleWorldSize;
	static glm::vec2 initialCloud1Pos;
	static glm::vec2 initialCloud2Pos;

	static SoundAsset* s_gameMusic = nullptr;

	void CrappyBird::CrappyBird::OnAppStartUp()
	{
		CreateBackGround();
		CreatePlayer();
		CreateGround();
		SpawnObstacles();
		//disable phys debug draw
		AIngine::World::SetPhysicsDebugDrawActive(false);

		// load game music
		std::string soundPath = "assets/CrappyBird/sounds/Off_Limits.wav";
		s_gameMusic = Assets::Load<SoundAsset>(soundPath);
		s_gameMusic->GetSound().looped = true;
		Sounds::Play(*s_gameMusic);
	}


	void CrappyBird::CrappyBird::OnAppShutDown()
	{
		s_gameMusic->GetSound().paused = true;
	}

	static float s_Score = 0.0f;
	static float s_worldSpeed = 0.025f;

	void CrappyBird::CrappyBird::OnAppUpdate()
	{
		if (m_running) {

			s_Score += s_worldSpeed;

			if (AIngine::Input::IsMouseButtonPressed(0) && m_running)
			{
				m_player->GetComponent<AIngine::Physics::PhysicsComponent>()->ApplyLinearImpulseToCenter(glm::vec2(0, -0.075f));
			}

			MoveObstacles();
			UpdateBackGround();

			if (isGameOver()) {
				m_running = false;
				m_player->GetComponent<AIngine::Physics::PhysicsComponent>()->SetActive(false);
			}
		}

		std::stringstream ss;
		ss << "Score: " << s_Score;
		AIngine::Graphics::Text(ss.str().c_str(), glm::vec2(20, 30), glm::vec2(2.5), glm::vec3(0, 0, 1));
	}

	void CrappyBird::OnAppEvent(AIngine::Events::Event & e)
	{
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent)) {
			AIngine::Events::KeyPressedEvent pressedEvent = dynamic_cast<AIngine::Events::KeyPressedEvent&>(e);
			if (pressedEvent.GetKeyCode() == AIngine::KeyCodes::R) {
				if (!m_running)
					RestartGame();
			}
		}
	}

	CrappyBird::CrappyBird()
	{
		// configure game world
		m_gravity = (glm::vec2(0.0, 5.0));
		m_bounds = glm::vec4(0.0, 10.0, 0.0, 10.0);

		GLFWmonitor* primary = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primary);
		m_windowConfig.Height = mode->height;
		m_windowConfig.Width = mode->width;
		m_windowConfig.Title = std::string("CrappyBird");
	}

	CrappyBird::~CrappyBird()
	{
	}


	void CrappyBird::CreateGround()
	{
		// spawn bottom ground
		AIngine::GameObject* ground = AIngine::World::SpawnObject();
		ground->SetName("GroundBottom");
		ground->SetLocalPosition(glm::vec2(5, 5));
		m_bottomGroundPosY = ground->GetLocalPosition().y;

		AIngine::Physics::PhysicsComponent* phys = ground->AddComponent<AIngine::Physics::PhysicsComponent>();
		AIngine::Physics::PhysicsProperties properties;
		properties.density = 1.0f;
		phys->CreateEdgeBody(properties, AIngine::Physics::PhysicsBodyType::e_Static, glm::vec2(-5, 0), glm::vec2(5, 0));

		// spawn ceiling
		ground = AIngine::World::SpawnObject();
		ground->SetName("Ceiling");
		ground->SetLocalPosition(glm::vec2(5, 0));
		m_ceilingPosY = ground->GetLocalPosition().y;
		phys = ground->AddComponent<AIngine::Physics::PhysicsComponent>();
		phys->CreateEdgeBody(properties, AIngine::Physics::PhysicsBodyType::e_Static, glm::vec2(-5, 0), glm::vec2(5, 0));
	}

	void CrappyBird::CreatePlayer()
	{
		m_player = m_world->SpawnObject(std::string("Player"), nullptr, glm::vec2(1.5, 3));

		std::string circleImagePath("assets/Intellgine/textures/Circle.png");
		AIngine::Rendering::Bitmap* circleBitmap = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(circleImagePath)->GetBitmap();
		AIngine::Rendering::Texture2D* texture = m_player->AddComponent<AIngine::Rendering::Texture2D>();
		texture->Generate(*circleBitmap);
		texture->SetLocalWorldSize(glm::vec2(0.5f));
		m_playerSize = texture->GetLocalWorldSize();
		texture->SetColor(glm::vec3(0.2f, 0.2f, 1.0f));

		AIngine::Physics::PhysicsComponent* physComp = m_player->AddComponent<AIngine::Physics::PhysicsComponent>();
		float32 radius = m_playerSize.x / 2.0f;
		AIngine::Physics::PhysicsProperties properties;
		properties.density = 1.0f;
		physComp->CreateCircleBody(properties, AIngine::Physics::PhysicsBodyType::e_Dynamic, radius);
	}

	static float obstacleWidth = 1.25f;

	void CrappyBird::SpawnObstacles()
	{
		std::string boxImagePath("assets/Intellgine/textures/White.png");
		AIngine::Rendering::Bitmap* boxBitmap = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(boxImagePath)->GetBitmap();
		glm::vec4 worldBounds = AIngine::World::GetBounds();
		srand(time(NULL));
		float minObstacleHeight = 1.0f;
		float maxObstacleHeight = (m_bottomGroundPosY / 2.0f) - (2.5f* m_playerSize.y);
		float minDistance = obstacleWidth;
		float maxDistance = 1.5f * obstacleWidth;
		glm::vec2 spawnPosition(0);
		m_obstacleParent = AIngine::World::SpawnObject(std::string("Obstacles"), nullptr);

		// color
		glm::vec3 color(1.0);
		glm::vec2 spawnPosMin(1);
		glm::vec2 spawnPosMax(1);
		spawnPosMin.x = worldBounds.x + m_playerSize.x * 4.0f + obstacleWidth;
		spawnPosMax.x = (4.0f* worldBounds.y) - obstacleWidth / 2.0f;

		AIngine::GameObject* spawnedObject;
		int count = 0;

		for (float spawnPosX = spawnPosMin.x; spawnPosX <= spawnPosMax.x; spawnPosX += minDistance + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / maxDistance))
		{
			color.x = 0.1f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			color.y = 0.1f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			color.z = 0.1f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			spawnPosition.x = spawnPosX;
			float sizeY = minObstacleHeight + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / maxObstacleHeight);
			spawnPosition.y = sizeY / 2.0f;

			spawnedObject = AIngine::World::SpawnObject(std::string("Obstacle").append(std::to_string(count)), m_obstacleParent, spawnPosition, glm::vec2(1.0));

			// add texture
			{
				using namespace AIngine::Rendering;
				Texture2D* texture = spawnedObject->AddComponent<Texture2D>();
				texture->Filter_Max = GL_NEAREST;
				texture->Filter_Min = GL_NEAREST;
				texture->Generate(*boxBitmap);
				texture->SetLocalWorldSize(glm::vec2(obstacleWidth, sizeY));
				texture->SetColor(color);
			}

			// add physics
			AIngine::Physics::PhysicsComponent* physComponent = spawnedObject->AddComponent<AIngine::Physics::PhysicsComponent>();
			AIngine::Physics::PhysicsProperties properties;
			physComponent->CreateBoxBody(properties, AIngine::Physics::PhysicsBodyType::e_Static, obstacleWidth, sizeY);

			count++;

		}

		for (float spawnPosX = spawnPosMin.x; spawnPosX <= spawnPosMax.x; spawnPosX += minDistance + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / maxDistance))
		{
			color.x = 0.1f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			color.y = 0.1f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			color.z = 0.1f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			spawnPosition.x = spawnPosX;
			float sizeY = minObstacleHeight + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / maxObstacleHeight);
			spawnPosition.y = m_bottomGroundPosY - (sizeY / 2.0f);

			spawnedObject = AIngine::World::SpawnObject(std::string("Obstacle").append(std::to_string(count)), m_obstacleParent, spawnPosition, glm::vec2(1.0));

			// add texture
			{
				using namespace AIngine::Rendering;
				Texture2D* texture = spawnedObject->AddComponent<Texture2D>();
				texture->Filter_Max = GL_NEAREST;
				texture->Filter_Min = GL_NEAREST;
				texture->Generate(*boxBitmap);
				texture->SetLocalWorldSize(glm::vec2(obstacleWidth, sizeY));
				texture->SetColor(color);
			}

			// add physics
			AIngine::Physics::PhysicsComponent* physComponent = spawnedObject->AddComponent<AIngine::Physics::PhysicsComponent>();
			AIngine::Physics::PhysicsProperties properties;
			physComponent->CreateBoxBody(properties, AIngine::Physics::PhysicsBodyType::e_Static, obstacleWidth, sizeY);

			count++;

		}

	}

	void CrappyBird::CreateBackGround()
	{
		using assets = AIngine::Assets::AssetRegistry;

		using BitmapAsset = AIngine::Assets::BitmapAsset;
		using string = std::string;
		using GameObject = AIngine::GameObject;
		using Texture2D = AIngine::Rendering::Texture2D;

		BitmapAsset* skyBitmap = assets::Load<BitmapAsset>(string("assets/CrappyBird/textures/hills-sky.png"));
		BitmapAsset* backGroundHills[4];
		BitmapAsset* backgroundCloudsBitmap = assets::Load<BitmapAsset>(string("assets/CrappyBird/textures/hills-clouds.png"));
		backGroundHills[0] = assets::Load<BitmapAsset>(string("assets/CrappyBird/textures/hills-1.png"));
		backGroundHills[1] = assets::Load<BitmapAsset>(string("assets/CrappyBird/textures/hills-2.png"));
		backGroundHills[2] = assets::Load<BitmapAsset>(string("assets/CrappyBird/textures/hills-3.png"));
		backGroundHills[3] = assets::Load<BitmapAsset>(string("assets/CrappyBird/textures/hills-4.png"));

		VisibleWorldSize = m_camera->GetVisibleWorldSize();
		s_cloudRepeatWidth = VisibleWorldSize.x * 0.5f;

		// spawn sky
		GameObject* spawnedGameObject = m_world->SpawnObject(string("Sky"));
		m_backGroundSky = spawnedGameObject->AddComponent<Texture2D>();
		m_backGroundSky->Filter_Max = GL_NEAREST;
		m_backGroundSky->Filter_Min = GL_NEAREST;
		m_backGroundSky->Generate(skyBitmap->GetBitmap());
		m_backGroundSky->SetLocalWorldSize(VisibleWorldSize);
		spawnedGameObject->SetLocalPosition(VisibleWorldSize * 0.5f);

		// spawn clouds1
		spawnedGameObject = m_world->SpawnObject(string("Clouds"));
		m_backGroundClouds = spawnedGameObject->AddComponent<Texture2D>();
		m_backGroundClouds->Filter_Max = GL_NEAREST;
		m_backGroundClouds->Filter_Min = GL_NEAREST;
		m_backGroundClouds->Generate(backgroundCloudsBitmap->GetBitmap());
		m_backGroundClouds->SetLocalWorldSize(VisibleWorldSize);
		spawnedGameObject->SetLocalPosition(VisibleWorldSize * 0.5f);
		initialCloud1Pos = spawnedGameObject->GetLocalPosition();

		// spawn clouds2
		spawnedGameObject = m_world->SpawnObject(string("Clouds2"));
		m_backGroundClouds2 = spawnedGameObject->AddComponent<Texture2D>();
		m_backGroundClouds2->Filter_Max = GL_NEAREST;
		m_backGroundClouds2->Filter_Min = GL_NEAREST;
		m_backGroundClouds2->Generate(backgroundCloudsBitmap->GetBitmap());
		m_backGroundClouds2->SetLocalWorldSize(VisibleWorldSize);
		spawnedGameObject->SetLocalPosition(glm::vec2(VisibleWorldSize.x * 0.5f, VisibleWorldSize.y * 0.4f));
		initialCloud2Pos = spawnedGameObject->GetLocalPosition();


		for (int hillIndex = 0; hillIndex < 4; hillIndex++) {

			static glm::vec2 spawnPos = glm::vec2(VisibleWorldSize.x * 0.5f, 3.0f);

			spawnedGameObject = m_world->SpawnObject(string("Hills-").append(std::to_string(hillIndex)));
			m_backGroundHills[hillIndex] = spawnedGameObject->AddComponent<Texture2D>();
			m_backGroundHills[hillIndex]->Filter_Max = GL_NEAREST;
			m_backGroundHills[hillIndex]->Filter_Min = GL_NEAREST;
			m_backGroundHills[hillIndex]->Generate(backGroundHills[hillIndex]->GetBitmap());
			m_backGroundHills[hillIndex]->SetParallaxFactor(glm::vec2(1.0f) * 0.25f *(float)hillIndex);
			m_backGroundHills[hillIndex]->SetLocalWorldSize(glm::vec2(VisibleWorldSize.x, 6.0));
			spawnedGameObject->SetLocalPosition(spawnPos);
		}

	}

	void CrappyBird::UpdateBackGround()
	{
		s_cloudOffset -= GetDeltaTime() / 5.0f;
		if (s_cloudOffset < -s_cloudRepeatWidth)
			s_cloudOffset = s_cloudRepeatWidth;

		// move cloud 1
		glm::vec2 cloudPos = m_backGroundClouds->GetOwner()->GetLocalPosition();
		m_backGroundClouds->GetOwner()->SetLocalPosition(initialCloud1Pos + glm::vec2(-s_cloudOffset, 0.0f));

		//move cloud2
		cloudPos = m_backGroundClouds2->GetOwner()->GetLocalPosition();
		m_backGroundClouds2->GetOwner()->SetLocalPosition(initialCloud2Pos + glm::vec2(s_cloudOffset, 0.0f));
	}

	void CrappyBird::MoveObstacles()
	{
		auto it = m_obstacleParent->GetChildren().begin();
		glm::vec2 movement = glm::vec2(s_worldSpeed, 0);
		glm::vec4 worldBounds = AIngine::World::GetBounds();


		while (it != m_obstacleParent->GetChildren().end()) {
			glm::vec2 pos = (*it._Ptr)->GetLocalPosition();
			pos -= movement;
			if (pos.x < worldBounds.x - obstacleWidth)
				pos.x = (worldBounds.y * 4.0f) - 0.5f * obstacleWidth;
			(*it._Ptr)->SetLocalPosition(pos);
			it++;
		}
	}

	void CrappyBird::RestartGame()
	{
		m_running = true;
		m_player->GetComponent<AIngine::Physics::PhysicsComponent>()->SetActive(true);
		m_player->SetLocalPosition(glm::vec2(1.5, 3), true);
		m_player->GetComponent<AIngine::Physics::PhysicsComponent>()->SetCollision(false, nullptr);
		s_Score = 0.0f;

		for (auto child : m_obstacleParent->GetChildren()) {

			child->Translate(glm::vec2(AIngine::World::GetBounds().y / 2.0, 0));
		}
	}

	bool CrappyBird::isGameOver()
	{
		return (m_player->GetComponent<AIngine::Physics::PhysicsComponent>()->IsCollided());
	}

}