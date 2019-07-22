#include "GameManager.h"
#include "Pong.h"

#include "imgui.h"

namespace Pong {

	//glm::vec2 GameManager::ForceOnBall(0.7, 0.5);

	// Constructor
	GameManager::GameManager()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
		Pong::FIXED_TIMESTEP = 1.0 / 120.0;
		Pong::UseFixedTimeStep(true);
	}

	// Start is called when gameplay starts for this script
	void GameManager::OnStart()
	{
	}

	// End is called when gameplay ends for this script
	void GameManager::OnEnd()
	{
	}

	// Update is called once per frame
	void GameManager::Update(float delta)
	{
	}

	// Callback for events
	void GameManager::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void GameManager::OnWidget()
	{
		float gamespeed = (float)(Pong::Get().AppSpeedMulitplier);

		if (ImGui::DragFloat("GameSpeed", &gamespeed, 0.1, 0.5f, 10.0f)) {
			Pong::Get().AppSpeedMulitplier = gamespeed;
		}
	}
}