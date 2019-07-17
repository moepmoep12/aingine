#include "Edge.h"
namespace Forklifts {

	// Constructor
	Edge::Edge()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void Edge::OnStart()
	{
	}

	// End is called when gameplay ends for this script
	void Edge::OnEnd()
	{
	}

	// Update is called once per frame
	void Edge::Update(float delta)
	{
	}

	// Callback for events
	void Edge::OnEventData(AIngine::Events::EventData & e)
	{
	}
}