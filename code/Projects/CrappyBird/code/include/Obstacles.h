#pragma once
#include "AIngine/Core.h"
#include "Player.h"
#include "PickUp.h"
#include "PickUpFactory.h"

namespace CrappyBird {
	class Obstacles : public AIngine::Script {
	public:
		Obstacles();
		virtual ~Obstacles();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		void SpawnObstaclesInArea(const AIngine::Structures::RectangleI& worldRect);

	private:
		GameObject* GetAvailableObstacle();
		int m_lastObstacleHeight = -1;

	private:
		std::vector<AIngine::Rendering::Texture2D> m_obstacleTextures;
		AIngine::Events::EventHandler<void, AIngine::Structures::RectangleI&> m_newScreenHandler;
		Player* m_player;
		PickUpFactory* m_pickUpFactory;
	};
}