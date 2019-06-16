#pragma once

#include "AIngine/Core.h"
#include "Player.h"
#include "PickUp.h"

namespace CrappyBird {
	class Obstacles : public AIngine::Script {
	public:
		Obstacles() { SetName(typeid(*this).name()); }
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float deltatime) override;


		void SpawnObstaclesInArea(const AIngine::Structures::RectangleI& worldRect);

	private:
		GameObject* GetAvailableObstacle();
		int m_lastObstacleHeight = -1;

	private:
		AIngine::Events::EventHandler<void, AIngine::Structures::RectangleI&> m_newScreenHandler;
		Player* m_player;
		PickUpFactory* m_pickUpFactory;
	};
}