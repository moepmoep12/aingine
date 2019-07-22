#include "ForkliftAgent.h"

#include "imgui.h"
namespace Forklifts {

	// Constructor
	ForkliftAgent::ForkliftAgent()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void ForkliftAgent::OnStart()
	{
		Forklift::OnStart();
		m_experiment = GetParent()->GetComponent<Experiment>();
	}

	// End is called when gameplay ends for this script
	void ForkliftAgent::OnEnd()
	{
	}

	// Update is called once per frame
	void ForkliftAgent::Update(float delta)
	{
		if (currentTask)
			currentTask->Ticks++;

		MoveAlongPath();
	}

	// Callback for events
	void ForkliftAgent::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void ForkliftAgent::OnWidget()
	{
		ImGui::InputFloat("Speed", &maxSpeed);

		if (_currentNode) {
			ImGui::Text(_currentNode->Data().Name.c_str());
		}
		if (_nextNode) {
			ImGui::Text(_nextNode->Data().Name.c_str());
		}


	}

	std::vector<double> ForkliftAgent::Observe()
	{
		return std::vector<double>();
	}

	bool ForkliftAgent::IsEndOfProblem()
	{
		if (bTaskFinished)
		{
			bTaskFinished = false;
			return true;
		}
		return false;
	}

	void ForkliftAgent::ExecuteAction(int action)
	{
	}
	void ForkliftAgent::Reset()
	{
	}
	void ForkliftAgent::OnMaxStepsReached()
	{
	}

	void ForkliftAgent::OnNodeReached()
	{
		currentTask->visitedNodes++;

		_currentNode = _nextNode;

		//if (NodeInContact->node == _nextNode) {
		_nextNode = *path.begin()._Ptr;
		if (path.size() > 0)
			path.erase(path.begin());
		//}
	}

	void ForkliftAgent::FinishTask()
	{
		_currentNode = _nextNode;
		currentTask = nullptr;
		_nextNode = nullptr;
		OnTaskFinishedEvent(this);

	}

	void ForkliftAgent::ProcessTask()
	{
		_nextNode = *path.begin()._Ptr;
		path.erase(path.begin());
	}


	void ForkliftAgent::OnCollisionBegin(AIngine::Physics::Contact contact)
	{
		if (contact.Other) {
			for(auto& comp : contact.Other->GetComponents())
				if (dynamic_cast<Forklift*>(comp))
				{
					m_experiment->AmountCollisions++;
					break;
				}
		}
	}
}