#include "Pong.h"
#include "EntryPoint.h" // Do not remove

// Define the extern declaration
AIngine::Application* AIngine::CreateApplication() {
	return new Pong::Pong();
}

namespace Pong {

	// Constructor
	Pong::Pong()
	{		// Configure the physics world with a gravity of 10 downwards
		m_gravity = glm::vec2(0, 10);

		// Set the bounds of the world to be 10 in width and 10 in height
		m_bounds = glm::vec4(0.0, 10.0, 0.0, 10.0);

		// Set the window title
		m_window->SetWindowTitle("Pong");

		// Set the window size to be the whole screen
		const glm::vec2 monitorResolution = m_window->GetMonitorResolution();
		m_window->SetWindowSize(monitorResolution.x, monitorResolution.y);
	}

	// StartUp is called after the scene and engine subsystems have been initialized
	void Pong::OnAppStartUp()
	{
	}

	// ShutDown is called before the scene and engine subystems have been shut down
	void Pong::OnAppShutDown()
	{
	}

	// Update is called once per frame
	void Pong::OnAppUpdate()
	{
	}

	// Here you can react to events
	void Pong::OnAppEvent(AIngine::Events::EventData & e)
	{
	}
}