#include "XCSAgent.h"
namespace Pong {

	// Constructor
	XCSAgent::XCSAgent()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void XCSAgent::OnStart()
	{
	}

	// End is called when gameplay ends for this script
	void XCSAgent::OnEnd()
	{
	}

	// Update is called once per frame
	void XCSAgent::Update(float delta)
	{
	}

	// Callback for events
	void XCSAgent::OnEventData(AIngine::Events::EventData & e)
	{
	}
	std::vector<double, std::allocator<double>> XCSAgent::Observe()
	{
		return std::vector<double, std::allocator<double>>();
	}
	bool XCSAgent::IsEndOfProblem()
	{
		return false;
	}
	void XCSAgent::ExecuteAction(int action)
	{
	}
	void XCSAgent::Reset()
	{
	}
	void XCSAgent::OnMaxStepsReached()
	{
	}
}