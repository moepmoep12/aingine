#include "Forklift.h"
#include "Node.h"
#include "Forklifts.h"

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
		OnCollisionBeginEventHandler = AIngine::Events::EventHandler<void, AIngine::Physics::Contact>(std::bind(&Forklift::OnCollisionBegin, this, std::placeholders::_1));
		OnCollisionEndEventHandler = AIngine::Events::EventHandler<void, AIngine::Physics::Contact>(std::bind(&Forklift::OnCollisionEnd, this, std::placeholders::_1));
		GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>()->OnCollisionBegin += OnCollisionBeginEventHandler;
		GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>()->OnCollisionBegin += OnCollisionEndEventHandler;
		GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>()->SetEnabled(true);

		m_graph = AIngine::World::GetGameObject("Graph")->GetComponent<Graph>();
	}

	// End is called when gameplay ends for this script
	void Forklift::OnEnd()
	{
		GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>()->OnCollisionBegin -= OnCollisionBeginEventHandler;
		GetOwner()->GetComponent<AIngine::Physics::PhysicsComponent>()->OnCollisionEnd -= OnCollisionEndEventHandler;
	}

	// Update is called once per frame
	void Forklift::Update(float delta)
	{

	}

	// Callback for events
	void Forklift::OnEventData(AIngine::Events::EventData & e)
	{
	}


	void Forklift::ReceiveTask(Task * task)
	{
		currentTask = task;
		path = task->Path;
		ProcessTask();
	}

	void Forklift::MoveAlongPath()
	{
		if (!_nextNode) return;

		if (NearlyEqual(m_graph->NodeMap[_nextNode]->GetOwner()->GetWorldPosition(), GetOwner()->GetWorldPosition(), minDistance)) {
			// reached final node
			if (_nextNode == currentTask->endNode->node) {
				FinishTask();
				return;
			}
			else {
				OnNodeReached();
			}
		}

		Move();
	}

	void Forklift::MoveAlongPathRecursively(float * amount)
	{
		if (!_nextNode || *amount <= std::numeric_limits<float>::epsilon()) return;

		glm::vec2 nextnodepos = m_graph->NodeMap[_nextNode]->GetOwner()->GetWorldPosition();
		glm::vec2 playerpos = GetOwner()->GetWorldPosition();

		if (NearlyEqual(nextnodepos, playerpos, minDistance))
		{
			// reached final node
			if (_nextNode == currentTask->endNode->node) {
				FinishTask();
				return;
			}
			else {
				OnNodeReached();
			}
		}

		Move(amount);
	}

	void Forklift::Move()
	{
		if (!_nextNode) return;

		glm::vec2 direction = glm::normalize(m_graph->NodeMap[_nextNode]->GetOwner()->GetWorldPosition() - GetOwner()->GetWorldPosition());
		glm::vec2 nextnodepos = m_graph->NodeMap[_nextNode]->GetOwner()->GetWorldPosition();
		float amount = 0;

		if (IsMissingTarget(direction, &amount)) {
			GetOwner()->SetWorldPosition(nextnodepos);
			MoveAlongPathRecursively(&amount);
			return;
		}
		else {
			GetOwner()->SetWorldPosition(GetOwner()->GetWorldPosition() + (direction * maxSpeed * (float)Forklifts::FIXED_TIMESTEP));
		}
	}

	void Forklift::Move(float * value)
	{
		if (!_nextNode) return;

		glm::vec2 nextnodepos = m_graph->NodeMap[_nextNode]->GetOwner()->GetWorldPosition();
		glm::vec2 playerpos = GetOwner()->GetWorldPosition();
		glm::vec2 direction = glm::normalize(nextnodepos - playerpos);

		float amount = 0;

		if (IsMissingTarget(direction, &amount)) {
			GetOwner()->SetWorldPosition(nextnodepos);
			MoveAlongPathRecursively(&amount);
			return;
		}
		else {
			GetOwner()->SetWorldPosition(playerpos + (direction * *value));
		}
	}

	bool Forklift::NearlyEqual(const glm::vec2 & a, const glm::vec2 & b, float tolerance)
	{
		return std::abs(a.x - b.x) <= tolerance
			&& std::abs(a.y - b.y) <= tolerance;
	}

	void Forklift::OnCollisionBegin(AIngine::Physics::Contact contact)
	{
		if (contact.Other && contact.Other->GetComponent<Node>())
		{
			NodeInContact = contact.Other->GetComponent<Node>();
		}
	}

	void Forklift::OnCollisionEnd(AIngine::Physics::Contact contact)
	{
		if (contact.Other && contact.Other->GetComponent<Node>())
		{
			NodeInContact = nullptr;
		}
	}

	bool Forklift::IsMissingTarget(const glm::vec2 & direction, float * amount, float value)
	{
		glm::vec2 nextnodePos = m_graph->NodeMap[_nextNode]->GetOwner()->GetWorldPosition();
		glm::vec2 currentPos = GetOwner()->GetWorldPosition();
		glm::vec2 nextPosition = value > std::numeric_limits<float>::epsilon() ? currentPos + direction * value
			: currentPos + (direction * maxSpeed *(float)Forklifts::FIXED_TIMESTEP);
		glm::vec2 newDirection = glm::normalize(nextPosition - nextnodePos);

		if (NearlyEqual(direction, newDirection, 0.01f)) {
			*amount = glm::distance(nextPosition, nextnodePos);
			return true;
		}
		*amount = 0;
		return false;
	}

}
namespace AIngine::Events {
	int EventHandler<void, Forklifts::Forklift*>::counter = 0;
}