#pragma once
#include "Node.h"
#include "Edge.h"

#include <vector>
#include <string>
#include <sstream>
namespace Forklifts {
	struct Task {
		Node* startNode;
		Node* endNode;
		std::vector<IGraphNode<NodeData, EdgeData>*> Path;
		float Cost;
		long Ticks = 0;
		int visitedNodes = 0;

		bool GoalTest(IGraphNode<NodeData, EdgeData>* n) {
			return endNode->node == n;
		}

		float Heuristic(IGraphNode<NodeData, EdgeData>* n) {
			return glm::distance(n->Data().Koords, endNode->node->Data().Koords);
		}

		std::string to_string() const
		{
			std::stringstream ss;
			ss << "(" << startNode->node->Data().Name << " | " << endNode->node->Data().Name;
			return ss.str();
		}

	};
}