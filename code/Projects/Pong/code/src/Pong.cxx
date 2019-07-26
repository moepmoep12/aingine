#include "Pong.h"
#include "EntryPoint.h" // Do not remove

// Define the extern declaration
AIngine::Application* AIngine::CreateApplication() {
	return new Pong::Pong();
}

namespace Pong {

	int Pong::Difficulty = 1;
	std::map<int, float> Pong::PlayerSizes;
	std::map<int, float> Pong::PlayerTwoSizes;
	std::map<int, float> Pong::DifficultiesSpeed;

	// Constructor
	Pong::Pong()
	{		// Configure the physics world with a gravity of 10 downwards
		m_gravity = glm::vec2(0, 0);

		// Set the bounds of the world to be 10 in width and height according to 16/9 format
		m_bounds = glm::vec4(0.0, 10.0, 0.0, 10.0 * (9.0 / 16.0));

		// Set the window title
		m_window->SetWindowTitle("Pong");
		//m_window->SetVSync(false);
		// Set the window size to be the whole screen
		const glm::vec2 monitorResolution = m_window->GetMonitorResolution();
		m_window->SetWindowSize(monitorResolution.x, monitorResolution.y);

		PlayerSizes[0] = 300.0 / 200.0;
		PlayerSizes[1] = 200.0 / 200.0;
		PlayerSizes[2] = 130.0 / 200.0;
		PlayerSizes[3] = 75.0 / 200.0;

		PlayerTwoSizes[0] = 150.0 / 200.0;
		PlayerTwoSizes[1] = 200.0 / 200.0;
		PlayerTwoSizes[2] = 250.0 / 200.0;
		PlayerTwoSizes[3] = 300.0 / 200.0;

		DifficultiesSpeed[0] = 1;
		DifficultiesSpeed[1] = 1.25f;
		DifficultiesSpeed[2] = 1.5f;
		DifficultiesSpeed[3] = 1.6f;

		Pong::FIXED_TIMESTEP = 1.0 / 120.0;
		Pong::UseFixedTimeStep(true);

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
		// key pressed
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {
			AIngine::Events::KeyPressedEvent::KeyPressedEventData pressedEvent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);

			if (pressedEvent.GetKeyCode() == AIngine::KeyCode::R) {
				bRender = !bRender;
			}
		}
	}
	void Pong::SetVsync(bool active)
	{
		m_window->SetVSync(active);
	}
}