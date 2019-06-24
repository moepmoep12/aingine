#include "CrappyBird.h"
#include "EntryPoint.h" // Do not remove

// Define the extern declaration
AIngine::Application* AIngine::CreateApplication() {
	return new CrappyBird::CrappyBird();
}

namespace CrappyBird {

	const float CrappyBird::s_originalGameSpeed = 2.5f;
	float CrappyBird::s_GameSpeed = CrappyBird::s_originalGameSpeed;

	// Constructor
	CrappyBird::CrappyBird()
	{		// Configure the physics world with a gravity of 10 downwards
		m_gravity = glm::vec2(0, 5);

		// Set the bounds of the world to be 10 in width and 10 in height
		m_bounds = glm::vec4(0.0, 10.0, 0.0, 10.0);

		// Set the window title
		m_window->SetWindowTitle("CrappyBird");

		// Set the window size to be the whole screen
		const glm::vec2 monitorResolution = m_window->GetMonitorResolution();
		m_window->SetWindowSize(monitorResolution.x, monitorResolution.y);
	}

	// StartUp is called after the scene and engine subsystems have been initialized
	void CrappyBird::OnAppStartUp()
	{
	}

	// ShutDown is called before the scene and engine subystems have been shut down
	void CrappyBird::OnAppShutDown()
	{
	}

	// Update is called once per frame
	void CrappyBird::OnAppUpdate()
	{
	}

	// Here you can react to events
	void CrappyBird::OnAppEvent(AIngine::Events::EventData & e)
	{
	}
}