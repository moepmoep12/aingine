#pragma once
#include "Node.h"
#include "Edge.h"

namespace Forklifts {
	struct Task {
		IGraphNode<NodeData, EdgeData>* startNode;
		IGraphNode<NodeData, EdgeData>* endNode;
		int Ticks = 0;
		int visitedNodes = 0;
	};
}