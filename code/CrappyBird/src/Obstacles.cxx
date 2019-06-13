#include "Obstacles.h"
#include "AIngine/Core.h"

namespace CrappyBird {
	CrappyBird::Obstacles::Obstacles(GameObject * owner)
		: Script(owner)
	{
		SetName(typeid(*this).name());
	}

	void CrappyBird::Obstacles::OnStart()
	{

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