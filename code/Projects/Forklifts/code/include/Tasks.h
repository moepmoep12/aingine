#pragma once
#include "Node.h"
#include "Edge.h"

namespace Forklifts {
	struct Task {
		Node* startNode;
		Node* endNode;
		int Ticks = 0;
		int visitedNodes = 0;
	};
}