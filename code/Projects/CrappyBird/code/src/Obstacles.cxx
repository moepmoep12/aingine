#include "Obstacles.h"
#include "CrappyBird.h"
#include "Obstacle.h"
#include "AgentXCSR.h"

#include <cmath>
#include <math.h>
#include <random>
#include <time.h>

namespace CrappyBird {

	std::vector<std::vector<Quadrant>> Obstacles::s_CurrentMap;

	// Constructor
	Obstacles::Obstacles()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());

		// load obstacle textures
		for (int i = 1; i < 7; i++) {
			std::string path;
			path += "textures/set8_example_";
			path += std::to_string(i);
			path += ".png";
			m_obstacleTextures.push_back(AIngine::Rendering::Texture2D(Assets::Load<BitmapAsset>(path)->GetBitmap()));
		}
	}

	Obstacles::~Obstacles()
	{
		m_obstacleTextures.clear();
	}

	// Start is called when gameplay starts for this script
	void Obstacles::OnStart()
	{
		m_newScreenHandler = AIngine::Events::EventHandler<void, AIngine::Structures::RectangleF&>(std::bind(&Obstacles::SpawnObstaclesInArea, this, std::placeholders::_1));
		m_player = CrappyBird::s_AgentLearning ? AIngine::World::GetGameObject("AgentRocket")->GetComponent<AgentXCSR>() : AIngine::World::GetGameObject("PlayerRocket")->GetComponent<Player>();
		m_player->OnEnterNewScreen += m_newScreenHandler;
		m_pickUpFactory = AIngine::World::GetGameObject("PickUpFactory")->GetComponent<PickUpFactory>();
	}

	// End is called when gameplay ends for this script
	void Obstacles::OnEnd()
	{
		m_player->OnEnterNewScreen -= m_newScreenHandler;
		ResetMap();
	}

	// Update is called once per frame
	void Obstacles::Update(float delta)
	{
		if (m_player->IsGameOver) return;
		for (auto& child : GetOwner()->GetChildren()) {
			if (child->IsActive()) {
				child->Translate(glm::vec2(-CrappyBird::s_GameSpeed * delta, 0), true);
				if (child->GetWorldPosition().x < -child->GetComponent<Obstacle>()->GetRectangle().width) {
					//child->RemoveComponent<Obstacle>();
					child->SetActive(false);
					child->GetComponent<PhysicsComponent>()->SetEnabled(false);
				}
			}
		}
	}

	// Callback for events
	void Obstacles::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void Obstacles::SpawnObstaclesInArea(const AIngine::Structures::RectangleF & worldRect)
	{
		static std::default_random_engine generator;
		static std::normal_distribution<double> distribution(0, 0.25);

		static const glm::vec2 playerSize(2, 1);
		int QuadrantsX = floor((float)worldRect.width / playerSize.x);
		int QuadrantsY = floor((float)worldRect.height / playerSize.y);
		int textureIndex = rand() % (m_obstacleTextures.size() - 1);

		// if it's the first screen, choose a random height
		if (m_lastObstacleHeight == -1) {
			m_lastObstacleHeight = rand() % QuadrantsY;
			InitMap(worldRect);
		}
		else {
			ResetMap();
		}

		int index = -1;
		int heightIndex = m_lastObstacleHeight;
		std::vector<glm::vec2> Options;
		std::vector<Quadrant*> path;

		// loop for creating a player path through the next screen
		// a player path consists of quadrants where no obstacle can spawn
		while (index < QuadrantsX - 1)
		{
			if (index >= 0) {
				glm::vec2 currentpos(index, heightIndex);

				// up is possible
				if (heightIndex - 1 >= 0)
				{
					// quadrant direct above
					if (!s_CurrentMap[index][heightIndex - 1].isPlayerPath)
						Options.push_back(glm::vec2(index, heightIndex - 1));

					// quadrant right-above
					if (index + 1 < QuadrantsX) {
						if (!s_CurrentMap[index + 1][heightIndex - 1].isPlayerPath)
							Options.push_back(glm::vec2(index + 1, heightIndex - 1));
					}
				}

				// down is possible
				if (heightIndex + 1 < QuadrantsY) {

					// quadrant direct below
					if (!s_CurrentMap[index][heightIndex + 1].isPlayerPath)
						Options.push_back(glm::vec2(index, heightIndex + 1));

					// quadrant right-below
					if (index + 1 < QuadrantsX) {
						if (!s_CurrentMap[index + 1][heightIndex + 1].isPlayerPath)
							Options.push_back(glm::vec2(index + 1, heightIndex + 1));
					}
				}

				// right is possible
				if (index + 1 < QuadrantsX) {
					// quadrant to the right
					if (!s_CurrentMap[index + 1][heightIndex].isPlayerPath)
						Options.push_back(glm::vec2(index + 1, heightIndex));
				}
			}
			else
			{
				glm::vec2 currentpos(-1, heightIndex);
				// option to the right
				glm::vec2 nextpos = currentpos + glm::vec2(1, 0);
				Options.push_back(nextpos);
				// option right-up
				if (nextpos.y - 1 >= 0)
					Options.push_back(glm::vec2(nextpos.x, nextpos.y - 1));
				// option right-down
				if (nextpos.y + 1 < QuadrantsY)
					Options.push_back(glm::vec2(nextpos.x, nextpos.y + 1));
			}

			glm::vec2 chosenOption = Options[rand() % Options.size()];

			// if we take a diagonal path we also take the direct path right
			if (chosenOption.x == index + 1 && chosenOption.y != heightIndex) {
				s_CurrentMap[index + 1][heightIndex].isPlayerPath = true;
				path.push_back(&s_CurrentMap[index + 1][heightIndex]);
			}

			m_lastObstacleHeight = chosenOption.y;
			index = chosenOption.x;
			heightIndex = chosenOption.y;
			s_CurrentMap[index][heightIndex].isPlayerPath = true;
			path.push_back(&s_CurrentMap[index][heightIndex]);
			Options.clear();
		}

		// now spawn the obstacles
		float rotation = distribution(generator);
		int pathHeightIndex = 0;
		float maxHeight = 0;
		float minHeight = 0;
		float chosenHeight = 0;

		// obstacles from top to bottom
		for (int i = 0; i < QuadrantsX; i++) {
			for (int j = 0; j < QuadrantsY; j++) {
				if (s_CurrentMap[i][j].isPlayerPath) {
					pathHeightIndex = j;
					break;
				}
			}

			if (pathHeightIndex == 0) {
				continue;
			}

			maxHeight = s_CurrentMap[i][pathHeightIndex].rectangle.GetPosition().y;
			minHeight = maxHeight - 0.75f * playerSize.y;
			chosenHeight = minHeight + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxHeight - minHeight)));

			GameObject* obj = GetAvailableObstacle();
			if (obj) {
				obj->SetActive(true);
				obj->GetComponent<Sprite>()->SetTexture(m_obstacleTextures[textureIndex]);
				obj->GetComponent<PhysicsComponent>()->SetEnabled(true);
				obj->GetComponent<Obstacle>()->Set(AIngine::Structures::RectangleF
					{
						s_CurrentMap[i][0].rectangle.x,
						s_CurrentMap[i][0].rectangle.y,
						s_CurrentMap[i][0].rectangle.width,
						chosenHeight
					});
				if (CrappyBird::s_bObstacleRotation)
					obj->SetRotation(rotation);
			}
		}

		for (int i = 0; i < QuadrantsX; i++) {
			for (int j = QuadrantsY - 1; j >= 0; j--) {
				if (s_CurrentMap[i][j].isPlayerPath) {
					pathHeightIndex = j;
					break;
				}
			}

			if (pathHeightIndex == QuadrantsY - 1) {
				continue;
			}

			maxHeight = QuadrantsY * playerSize.y - s_CurrentMap[i][pathHeightIndex].rectangle.GetMax().y;
			minHeight = maxHeight - 0.75f * playerSize.y;
			chosenHeight = minHeight + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxHeight - minHeight)));

			GameObject* obj = GetAvailableObstacle();
			if (obj) {
				obj->SetActive(true);
				obj->GetComponent<Sprite>()->SetTexture(m_obstacleTextures[textureIndex]);
				obj->GetComponent<PhysicsComponent>()->SetEnabled(true);
				obj->GetComponent<Obstacle>()->Set(AIngine::Structures::RectangleF
					{
						s_CurrentMap[i][0].rectangle.x,
						5.625f - chosenHeight,
						s_CurrentMap[i][0].rectangle.width,
						chosenHeight
					});
				if (CrappyBird::s_bObstacleRotation)
					obj->SetRotation(rotation);
			}
		}

		static std::normal_distribution<float> pickUpDistribution(0.0f, 1.0f);


		int pickUpIndex = rand() % (path.size() - 1);
		m_pickUpFactory->SpawnPickUp(path[pickUpIndex]->rectangle.GetCenter());


		//float spawnAmount = distribution(generator);

		//for (float i = 0; i < spawnAmount; i++) {
		//	int pickUpIndex = rand() % (path.size() - 1);
		//	m_pickUpFactory->SpawnPickUp(path[pickUpIndex]->rectangle.GetCenter());
		//	path.erase(path.begin() + pickUpIndex);
		//}
	}

	void Obstacles::ResetMap()
	{
		if (s_CurrentMap.empty()) return;
		for (int i = 0; i < s_CurrentMap.size(); i++) {
			for (int j = 0; j < s_CurrentMap[i].size(); j++) {
				s_CurrentMap[i][j].isClosed = false;
				s_CurrentMap[i][j].isPlayerPath = false;
			}
		}
	}

	void Obstacles::InitMap(const AIngine::Structures::RectangleF& worldRect)
	{
		static const glm::vec2 playerSize(2, 1);
		int QuadrantsX = floor((float)worldRect.width / playerSize.x);
		int QuadrantsY = floor((float)worldRect.height / playerSize.y);
		// initialize s_CurrentMap
		for (int i = 0; i < QuadrantsX; i++) {
			s_CurrentMap.push_back(std::vector<Quadrant>());
			for (int j = 0; j < QuadrantsY; j++) {
				s_CurrentMap[i].push_back(Quadrant
					{
						i,
						j,
						AIngine::Structures::RectangleF(worldRect.x + i * playerSize.x, worldRect.y + j * playerSize.y,playerSize.x, playerSize.y),
						false,
						false
					});
			}
		}
	}

	GameObject * Obstacles::GetAvailableObstacle()
	{
		for (auto& child : GetOwner()->GetChildren()) {
			if (!child->IsActive())
				return child;
		}
		return nullptr;
	}
}