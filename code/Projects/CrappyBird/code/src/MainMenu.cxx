#include "MainMenu.h"
namespace CrappyBird {

	// Constructor
	MainMenu::MainMenu()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void MainMenu::OnStart()
	{
	}

	// End is called when gameplay ends for this script
	void MainMenu::OnEnd()
	{
	}

	// Update is called once per frame
	void MainMenu::Update(float delta)
	{
	}

	// Callback for events
	void MainMenu::OnEventData(AIngine::Events::EventData & e)
	{
	}
	void MainMenu::OnGUI()
	{
	}
}