#include "Obstacles.h"
#include "AIngine/Core.h"

namespace CrappyBird {
	CrappyBird::Obstacles::Obstacles()
	{
		SetName(typeid(*this).name());
	}

	void CrappyBird::Obstacles::OnStart()
	{
		m_playerSpawnPos = AIngine::World::GetGameObject("PlayerRocket")->GetWorldPosition();
	}

	void CrappyBird::Obstacles::OnEvent(AIngine::Events::EventData & e)
	{
	}

	void CrappyBird::Obstacles::Update(float deltatime)
	{
	}

	CrappyBird::Obstacles::~Obstacles()
	{
	}

}