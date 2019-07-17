#include "Forklift.h"
namespace Forklifts {

	// Constructor
	Forklift::Forklift()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void Forklift::OnStart()
	{
	}

	// End is called when gameplay ends for this script
	void Forklift::OnEnd()
	{
	}

	// Update is called once per frame
	void Forklift::Update(float delta)
	{
	}

	// Callback for events
	void Forklift::OnEventData(AIngine::Events::EventData & e)
	{
	}
}