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



	void CrappyBird::CrappyBird::OnAppStartUp()
	{
		CreateBackGround();
		CreatePlayer();
		CreateGround();
		SpawnObstacles();
	}


	void CrappyBird::CrappyBird::OnAppShutDown()
	{
	}


	void CrappyBird::CrappyBird::OnAppUpdate()
	{
		if (m_running) {
			MoveCamera();
			MoveObstacles();
			UpdateBackGround();

			if (m_player->GetComponent<AIngine::PhysicsComponent>()->IsCollided()) {
				m_running = false;
				AIngine::PhysicsComponent* other = static_cast<AIngine::PhysicsComponent*>(m_player->GetComponent<AIngine::PhysicsComponent>()->GetOtherCollider()->GetBody()->GetUserData());
				DEBUG_INFO(other->GetOwner()->GetName().c_str());
				m_player->RemoveComponent<AIngine::PhysicsComponent>();
			}
		}
	}

	void CrappyBird::OnAppEvent(AIngine::Events::Event & e)
	{
		if (typeid(e) == typeid(AIngine::Events::MouseScrolledEvent)) {
			AIngine::Events::MouseScrolledEvent scrolledEvent = dynamic_cast<AIngine::Events::MouseScrolledEvent&>(e);
			static float zoomSpeed = 30;
			m_camera->Zoom(scrolledEvent.GetYOffset() * GetDeltaTime() * zoomSpeed);
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

	void CrappyBird::MoveCamera() {

		// cam settings
		static float translationrate = 5.0f;
		static float rotationrate = 0.1f;
		static float zoomSpeed = 5.0f;

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::A))
		{
			m_camera->Translate(glm::vec2(-translationrate, 0.0));
		}

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::D))
		{
			m_camera->Translate(glm::vec2(translationrate, 0.0));
		}


		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::W))
		{
			m_camera->Translate(glm::vec2(0.0, -translationrate));
		}


		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::S))
		{
			m_camera->Translate(glm::vec2(0.0, +translationrate));
		}

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::E))
		{
			m_camera->Rotate(rotationrate *  D2R * GetDeltaTime());
		}

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::Q))
		{
			m_camera->Rotate(-rotationrate * D2R * GetDeltaTime());
		}

		if (AIngine::Input::IsKeyPressed(AIngine::KeyCodes::F))
		{
			glm::vec2 screenPos = glm::vec2(AIngine::Input::GetMouseX(), AIngine::Input::GetMouseY());

			m_camera->LookAt(m_camera->ScreenToWorldPoint(screenPos));
		}

		if (AIngine::Input::IsMouseButtonPressed(0) && m_running)
		{
			m_player->GetComponent<AIngine::PhysicsComponent>()->ApplyLinearImpulseToCenter(glm::vec2(0, -0.05f));
		}

	}

	void CrappyBird::CreateGround()
	{
		// spawn ground
		AIngine::GameObject* ground = AIngine::World::SpawnObject();
		ground->SetName("Ground");
		ground->SetLocalPosition(glm::vec2(5, 5));
		m_groundPosY = ground->GetLocalPosition().y;

		AIngine::PhysicsComponent* phys = ground->AddComponent<AIngine::PhysicsComponent>();

		b2BodyDef bodydef;
		bodydef.type = b2_staticBody;
		b2EdgeShape shape;
		shape.Set(b2Vec2(-5, 0), b2Vec2(5, 0));
		b2FixtureDef fixturedef;
		fixturedef.shape = &shape;
		fixturedef.density = 1.0;
		phys->CreateBody(bodydef, fixturedef);
	}

	void CrappyBird::CreatePlayer()
	{
		m_player = m_world->SpawnObject(std::string("Player"), nullptr, glm::vec2(1.5, 3));

		std::string circleImagePath("assets/Intellgine/textures/Circle.png");
		Bitmap* circleBitmap = &AIngine::Application::Get().GetAssetRegistry().Load<AIngine::Assets::BitmapAsset>(circleImagePath)->GetBitmap();
		AIngine::Rendering::Texture2D* texture = m_player->AddComponent<AIngine::Rendering::Texture2D>();
		texture->Generate(*circleBitmap);
		texture->SetLocalWorldSize(glm::vec2(0.5f));
		m_playerSize = texture->GetLocalWorldSize();
		texture->SetColor(glm::vec3(0.2f, 0.2f, 1.0f));

		AIngine::PhysicsComponent* physComp = m_player->AddComponent<AIngine::PhysicsComponent>();
		b2CircleShape circleShape;
		b2PolygonShape polyShape;
		b2BodyDef bodydef;
		bodydef.type = b2_dynamicBody;
		b2FixtureDef fixturedef;
		fixturedef.density = 1.0f;
		circleShape.m_radius = m_playerSize.x / 2.0f;
		fixturedef.shape = &circleShape;
		physComp->CreateBody(bodydef, fixturedef);
	}

	static float obstacleWidth = 1.25f;

	void CrappyBird::SpawnObstacles()
	{
		std::string boxImagePath("assets/Intellgine/textures/White.png");
		Bitmap* boxBitmap = &GetAssetRegistry().Load<AIngine::Assets::BitmapAsset>(boxImagePath)->GetBitmap();
		glm::vec4 worldBounds = AIngine::World::GetBounds();
		srand(time(NULL));
		float minObstacleHeight = 1.0f;
		float maxObstacleHeight = (m_groundPosY / 2.0f) - (2.5f* m_playerSize.y);
		float minDistance = obstacleWidth;
		float maxDistance = 1.5f * obstacleWidth;
		glm::vec2 spawnPosition(0);
		m_obstacleParent = AIngine::World::SpawnObject(std::string("Obstacles"), nullptr);

		// color
		glm::vec3 color(1.0);



		glm::vec2 spawnPosMin(1);
		glm::vec2 spawnPosMax(1);
		spawnPosMin.x = worldBounds.x + m_playerSize.x * 4.0f + obstacleWidth ;
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
				texture->Generate(*boxBitmap);
				texture->SetLocalWorldSize(glm::vec2(obstacleWidth, sizeY));
				texture->SetColor(color);
			}

			// add physics
			AIngine::PhysicsComponent* physComponent = spawnedObject->AddComponent<AIngine::PhysicsComponent>();
			b2PolygonShape polyShape;
			b2BodyDef bodydef;
			bodydef.type = b2_staticBody;
			b2FixtureDef fixturedef;
			fixturedef.density = 1.0f;
			polyShape.SetAsBox(obstacleWidth / 2.0f, sizeY / 2.0f);
			fixturedef.shape = &polyShape;
			physComponent->CreateBody(bodydef, fixturedef);

			count++;

		}

		for (float spawnPosX = spawnPosMin.x; spawnPosX <= spawnPosMax.x; spawnPosX += minDistance + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / maxDistance))
		{
			color.x = 0.1f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			color.y = 0.1f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			color.z = 0.1f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			spawnPosition.x = spawnPosX;
			float sizeY = minObstacleHeight + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / maxObstacleHeight);
			spawnPosition.y = m_groundPosY - (sizeY / 2.0f);

			spawnedObject = AIngine::World::SpawnObject(std::string("Obstacle").append(std::to_string(count)), m_obstacleParent, spawnPosition, glm::vec2(1.0));

			// add texture
			{
				using namespace AIngine::Rendering;
				Texture2D* texture = spawnedObject->AddComponent<Texture2D>();
				texture->Generate(*boxBitmap);
				texture->SetLocalWorldSize(glm::vec2(obstacleWidth, sizeY));
				texture->SetColor(color);
			}

			// add physics
			AIngine::PhysicsComponent* physComponent = spawnedObject->AddComponent<AIngine::PhysicsComponent>();
			b2PolygonShape polyShape;
			b2BodyDef bodydef;
			bodydef.type = b2_staticBody;
			b2FixtureDef fixturedef;
			fixturedef.density = 1.0f;
			polyShape.SetAsBox(obstacleWidth / 2.0f, sizeY / 2.0f);
			fixturedef.shape = &polyShape;
			physComponent->CreateBody(bodydef, fixturedef);

			count++;

		}

	}

	void CrappyBird::CreateBackGround()
	{
		AIngine::Assets::AssetRegistry& assets = GetAssetRegistry();

		using BitmapAsset = AIngine::Assets::BitmapAsset;
		using string = std::string;
		using GameObject = AIngine::GameObject;
		using Texture2D = AIngine::Rendering::Texture2D;

		BitmapAsset* skyBitmap = assets.Load<BitmapAsset>(string("assets/CrappyBird/textures/hills-sky.png"));
		BitmapAsset* backGroundHills[4];
		BitmapAsset* backgroundCloudsBitmap = assets.Load<BitmapAsset>(string("assets/CrappyBird/textures/hills-clouds.png"));
		backGroundHills[0] = assets.Load<BitmapAsset>(string("assets/CrappyBird/textures/hills-1.png"));
		backGroundHills[1] = assets.Load<BitmapAsset>(string("assets/CrappyBird/textures/hills-2.png"));
		backGroundHills[2] = assets.Load<BitmapAsset>(string("assets/CrappyBird/textures/hills-3.png"));
		backGroundHills[3] = assets.Load<BitmapAsset>(string("assets/CrappyBird/textures/hills-4.png"));

		VisibleWorldSize = m_camera->GetVisibleWorldSize();
		s_cloudRepeatWidth = VisibleWorldSize.x * 0.5f;

		// spawn sky
		GameObject* spawnedGameObject = m_world->SpawnObject(string("Sky"));
		m_backGroundSky = spawnedGameObject->AddComponent<Texture2D>();
		m_backGroundSky->Generate(skyBitmap->GetBitmap());
		m_backGroundSky->SetLocalWorldSize(VisibleWorldSize);
		spawnedGameObject->SetLocalPosition(VisibleWorldSize * 0.5f);

		// spawn clouds1
		spawnedGameObject = m_world->SpawnObject(string("Clouds"));
		m_backGroundClouds = spawnedGameObject->AddComponent<Texture2D>();
		m_backGroundClouds->Generate(backgroundCloudsBitmap->GetBitmap());
		m_backGroundClouds->SetLocalWorldSize(VisibleWorldSize);
		spawnedGameObject->SetLocalPosition(VisibleWorldSize * 0.5f);
		initialCloud1Pos = spawnedGameObject->GetLocalPosition();

		// spawn clouds2
		spawnedGameObject = m_world->SpawnObject(string("Clouds2"));
		m_backGroundClouds2 = spawnedGameObject->AddComponent<Texture2D>();
		m_backGroundClouds2->Generate(backgroundCloudsBitmap->GetBitmap());
		m_backGroundClouds2->SetLocalWorldSize(VisibleWorldSize);
		spawnedGameObject->SetLocalPosition(glm::vec2(VisibleWorldSize.x * 0.5f, VisibleWorldSize.y * 0.4f));
		initialCloud2Pos = spawnedGameObject->GetLocalPosition();


		for (int hillIndex = 0; hillIndex < 4; hillIndex++) {

			static glm::vec2 spawnPos = glm::vec2(VisibleWorldSize.x * 0.5f, 3.0f);

			spawnedGameObject = m_world->SpawnObject(string("Hills-").append(std::to_string(hillIndex)));
			m_backGroundHills[hillIndex] = spawnedGameObject->AddComponent<Texture2D>();
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
		glm::vec2 movement = glm::vec2(GetDeltaTime() / 2.0f, 0);
		glm::vec4 worldBounds = AIngine::World::GetBounds();


		while (it != m_obstacleParent->GetChildren().end()) {
			glm::vec2 pos = (*it._Ptr)->GetLocalPosition();
			pos -= movement;
			if (pos.x < worldBounds.x)
				pos.x = (worldBounds.y * 4.0f) - 0.5f * obstacleWidth;
			(*it._Ptr)->SetLocalPosition(pos);
			AIngine::PhysicsComponent* physComp = (*it._Ptr)->GetComponent<AIngine::PhysicsComponent>();
			physComp->UpdateTransform();
			it++;
		}
	}

}