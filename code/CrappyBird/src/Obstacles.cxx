#include "Obstacles.h"
#include "Player.h"
#include "CrappyBird.h"
#include "Obstacle.h"

#include <cmath>
#include <math.h>
#include <random>
#include <time.h>

namespace CrappyBird {

	void Obstacles::OnStart()
	{
		m_newScreenHandler = AIngine::Events::EventHandler<void, AIngine::Structures::RectangleI&>(std::bind(&Obstacles::SpawnObstaclesInArea, this, std::placeholders::_1));
		m_player = AIngine::World::GetGameObject("PlayerRocket")->GetComponent<Player>();
		m_player->OnEnterNewScreen += m_newScreenHandler;
		m_pickUpFactory = AIngine::World::GetGameObject("PickUpFactory")->GetComponent<PickUpFactory>();
	}

	void Obstacles::OnEnd()
	{
		m_player->OnEnterNewScreen -= m_newScreenHandler;
	}

	void Obstacles::Update(float deltatime)
	{
		for (auto& child : GetOwner()->GetChildren()) {
			if (child->IsActive()) {
				child->Translate(glm::vec2(-CrappyBird::s_GameSpeed * deltatime, 0), true);
				if (child->GetWorldPosition().x < -2 * child->GetComponent<Obstacle>()->GetRectangle().width) {
					child->RemoveComponent<Obstacle>();
					child->SetActive(false);
				}
			}
		}
	}

	struct Quadrant {
		int x;
		int	y;
		AIngine::Structures::RectangleF rectangle;
		bool isPlayerPath = false;
		bool isClosed = false;
	};


	void Obstacles::SpawnObstaclesInArea(const AIngine::Structures::RectangleI & worldRect)
	{
		glm::vec2 playerSize(2, 1);
		int QuadrantsX = floor((float)worldRect.width / playerSize.x);
		int QuadrantsY = floor((float)worldRect.height / playerSize.y);

		if (m_lastObstacleHeight == -1) {
			m_lastObstacleHeight = rand() % QuadrantsY;
		}

		std::vector<std::vector<Quadrant>> map;

		// initialize map
		for (int i = 0; i < QuadrantsX; i++) {
			map.push_back(std::vector<Quadrant>());
			for (int j = 0; j < QuadrantsY; j++) {
				map[i].push_back(Quadrant
					{
						i,
						j,
						AIngine::Structures::RectangleF(worldRect.x + i * playerSize.x, worldRect.y + j * playerSize.y,playerSize.x, playerSize.y),
						false,
						false
					});
			}
		}

		int index = -1;
		int heightIndex = m_lastObstacleHeight;
		std::vector<glm::vec2> Options;
		std::vector<Quadrant*> path;

		while (index < QuadrantsX - 1)
		{
			if (index >= 0) {
				glm::vec2 currentpos(index, heightIndex);

				// up is possible
				if (heightIndex - 1 >= 0)
				{
					// quadrant direct above
					if (!map[index][heightIndex - 1].isPlayerPath)
						Options.push_back(glm::vec2(index, heightIndex - 1));

					// quadrant right-above
					if (index + 1 < QuadrantsX) {
						if (!map[index + 1][heightIndex - 1].isPlayerPath)
							Options.push_back(glm::vec2(index + 1, heightIndex - 1));
					}
				}

				// down is possible
				if (heightIndex + 1 < QuadrantsY) {

					// quadrant direct below
					if (!map[index][heightIndex + 1].isPlayerPath)
						Options.push_back(glm::vec2(index, heightIndex + 1));

					// quadrant right-below
					if (index + 1 < QuadrantsX) {
						if (!map[index + 1][heightIndex + 1].isPlayerPath)
							Options.push_back(glm::vec2(index + 1, heightIndex + 1));
					}
				}

				// right is possible
				if (index + 1 < QuadrantsX) {
					// quadrant to the right
					if (!map[index + 1][heightIndex].isPlayerPath)
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
				map[index + 1][heightIndex].isPlayerPath = true;
				path.push_back(&map[index + 1][heightIndex]);
			}

			m_lastObstacleHeight = chosenOption.y;
			index = chosenOption.x;
			heightIndex = chosenOption.y;
			map[index][heightIndex].isPlayerPath = true;
			path.push_back(&map[index][heightIndex]);
			Options.clear();
		}

		int pathHeightIndex = 0;
		float maxHeight = 0;
		float minHeight = 0;
		float chosenHeight = 0;

		// obstacles from top to bottom
		for (int i = 0; i < QuadrantsX; i++) {
			for (int j = 0; j < QuadrantsY; j++) {
				if (map[i][j].isPlayerPath) {
					pathHeightIndex = j;
					break;
				}
			}

			if (pathHeightIndex == 0) {
				continue;
			}

			maxHeight = map[i][pathHeightIndex].rectangle.GetPosition().y;
			minHeight = maxHeight - 0.75f * playerSize.y;
			chosenHeight = minHeight + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxHeight - minHeight)));

			GameObject* obj = GetAvailableObstacle();
			if (obj) {
				obj->SetActive(true);
				obj->AddComponent<Obstacle>()->Set(AIngine::Structures::RectangleF
					{
						map[i][0].rectangle.x,
						map[i][0].rectangle.y,
						map[i][0].rectangle.width,
						chosenHeight
					});
			}
		}

		for (int i = 0; i < QuadrantsX; i++) {
			for (int j = QuadrantsY - 1; j >= 0; j--) {
				if (map[i][j].isPlayerPath) {
					pathHeightIndex = j;
					break;
				}
			}

			if (pathHeightIndex == QuadrantsY - 1) {
				continue;
			}

			maxHeight = QuadrantsY * playerSize.y - map[i][pathHeightIndex].rectangle.GetMax().y;
			minHeight = maxHeight - 0.75f * playerSize.y;
			chosenHeight = minHeight + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxHeight - minHeight)));

			GameObject* obj = GetAvailableObstacle();
			if (obj) {
				obj->SetActive(true);
				obj->AddComponent<Obstacle>()->Set(AIngine::Structures::RectangleF
					{
						map[i][0].rectangle.x,
						5.0f - chosenHeight,
						map[i][0].rectangle.width,
						chosenHeight
					});
			}
		}

		std::normal_distribution<float> distribution(1.0f, 1.0f);
		std::default_random_engine generator;

		float spawnAmount = distribution(generator);

		for (float i = 0; i < spawnAmount; i++) {
			int pickUpIndex = rand() % (path.size() - 1);
			m_pickUpFactory->SpawnPickUp(path[pickUpIndex]->rectangle.GetCenter());
			path.erase(path.begin() + pickUpIndex);
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