#pragma once
#include "AIngine/Core.h"
#include "Tasks.h"
#include "Graph.h"

namespace Forklifts {
	class Forklift : public AIngine::Script {
	public:
		Forklift();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		Task* currentTask = nullptr;
		std::vector<IGraphNode<NodeData, EdgeData>*> path;

		virtual void ReceiveTask(Task* task);

		AIngine::Events::Event<void, Forklift*> OnTaskFinishedEvent;
		typedef AIngine::Events::EventHandler<void, Forklift*> OnTaskFinishedEventHandler;
		GraphNode* _currentNode = nullptr;
		GraphNode* _nextNode = nullptr;
		IEdge<EdgeData>* _currentEdge = nullptr;
		Node* NodeInContact = nullptr;
		int currentTaskIndex = 0;


	protected:
		virtual void MoveAlongPath();
		virtual void MoveAlongPathRecursively(float* amount);
		virtual void Move();
		virtual void Move(float* value);
		virtual void OnNodeReached() {}
		virtual void FinishTask() {}
		virtual void ProcessTask() {}

	protected:
		Graph* m_graph;
		float maxSpeed = 4;
		float minDistance = 0.06f;

	private:
		virtual void OnCollisionBegin(AIngine::Physics::Contact contact);
		virtual void OnCollisionEnd(AIngine::Physics::Contact contact);

		bool IsMissingTarget(const glm::vec2& direction, float* amount, float value = 0);
		bool NearlyEqual(const glm::vec2& a, const glm::vec2& b, float tolerance = 0.01f);

	private:
		AIngine::Events::EventHandler<void, AIngine::Physics::Contact> OnCollisionBeginEventHandler;
		AIngine::Events::EventHandler<void, AIngine::Physics::Contact> OnCollisionEndEventHandler;

	};
}