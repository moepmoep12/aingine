#pragma once
#include "AIngine/Core.h"
#include "Rendering/texture.h"
#include "Player.h"


namespace CrappyBird {

	class CrappyBird : public AIngine::Application {
	public:

		// Inherited via Application
		virtual void OnAppStartUp() override;
		virtual void OnAppShutDown() override;
		virtual void OnAppUpdate() override;
		virtual void OnAppEvent(AIngine::Events::EventData& e) override;

		CrappyBird();
		virtual ~CrappyBird();

	private:
		void CreateGround();
		void CreatePlayer();
		void SpawnObstacles();
		void CreateBackGround();
		void UpdateBackGround();
		void MoveObstacles();
		void RestartGame();

		bool isGameOver();

	private:
		AIngine::Rendering::Texture2D* m_backGroundSky;
		AIngine::Rendering::Texture2D* m_backGroundClouds;
		AIngine::Rendering::Texture2D* m_backGroundClouds2;
		AIngine::Rendering::Texture2D* m_backGroundHills[4];
		glm::vec2 m_playerSize;
		AIngine::GameObject* m_obstacleParent;
		AIngine::GameObject* m_player;
		float32 m_bottomGroundPosY;
		float32 m_ceilingPosY;
		bool m_running = true;
	};
}