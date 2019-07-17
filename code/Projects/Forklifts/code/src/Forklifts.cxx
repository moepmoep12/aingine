#include "Forklifts.h"
#include "EntryPoint.h" // Do not remove

// Define the extern declaration
AIngine::Application* AIngine::CreateApplication() {
	return new Forklifts::Forklifts();
}

namespace Forklifts {

	// Constructor
	Forklifts::Forklifts()
	{		
		// Configure the physics world with no gravity 
		m_gravity = glm::vec2(0, 0);

		// Set the bounds of the world to be 10 in width and 10 in height
		m_bounds = glm::vec4(0.0, 10.0, 0.0, 10.0);

		// Set the window title
		m_window->SetWindowTitle("Forklifts");

		// Set the window size to be the whole screen
		const glm::vec2 monitorResolution = m_window->GetMonitorResolution();
		m_window->SetWindowSize(monitorResolution.x, monitorResolution.y);

		UseFixedTimeStep(true);
		FIXED_TIMESTEP = 1.0 / 120.0;
	}

	// StartUp is called after the scene and engine subsystems have been initialized
	void Forklifts::OnAppStartUp()
	{
	}

	// ShutDown is called before the scene and engine subystems have been shut down
	void Forklifts::OnAppShutDown()
	{
	}

	// Update is called once per frame
	void Forklifts::OnAppUpdate()
	{
	}

	// Here you can react to events
	void Forklifts::OnAppEvent(AIngine::Events::EventData & e)
	{
	}
}