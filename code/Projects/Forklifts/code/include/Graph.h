#pragma once
#include "AIngine/Core.h"
#include "Edge.h"
#include "Node.h"
#include "Tasks.h"

#include <unordered_set>
#include <unordered_map>
#include <algorithm>

namespace Forklifts {

	template<typename N, typename E>
	class IGraph {
	public:
		inline ~IGraph() {
			for (auto node : nodes)
				delete node;
			for (auto edge : edges)
				delete edge;
		}
		inline void AddNode(IGraphNode<N, E>* node) {
			if (!ContainsNode(node)) {
				nodes.push_back(std::move(node));
			}
		}

		inline void AddEdgeBidirectional(IGraphNode<N, E>* u, IGraphNode<N, E>* v, IEdge<E>* edge) {
			if (!u || !v || !edge) return;
			if (ContainsEdge(edge)) return;
			if (!ContainsNode(u) || !ContainsNode(v)) return;

			u->AddEdge(v, edge);
			v->AddEdge(u, edge);
			edges.push_back(std::move(edge));
		}

		inline void AddEdgeDirectional(IGraphNode<N, E>* from, IGraphNode<N, E>* to, IEdge<E>* edge) {
			if (!from || !to || !edge) return;
			if (ContainsEdge(edge)) return;
			if (!ContainsNode(to) || !ContainsNode(from)) return;

			from->AddEdge(to, edge);
			edges.push_back(edge);
		}

		inline bool ContainsEdge(IEdge<E>* edge) {
			if (!edge) return false;
			return std::find(edges.begin(), edges.end(), edge) != edges.end();
		}

		inline bool ContainsEdge(const E& edgeData) {
			for (auto& edge : edges)
				if (edge.Data == edgeData) return true;
		}

		inline bool ContainsNode(IGraphNode<N, E>* node) {
			if (!node) return false;
			return std::find(nodes.begin(), nodes.end(), node) != nodes.end();
		}

		inline bool ContainsNode(const N& nodeData) {
			for (auto& node : nodes)
				if (node.Data() == nodeData) return true;
		}


		std::vector<IGraphNode<N, E>*> nodes;
		std::vector<IEdge<E>*> edges;
	};

	typedef IGraphNode<NodeData, EdgeData> GraphNode;

	class Graph : public AIngine::Script {
	public:
		Graph();
		virtual ~Graph();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		GraphNode* FindNode(const std::string& name);


	public:
		std::string nodeJsonPath = "Nodes.txt";
		std::string edgeJsonPath = "Edges.txt";
		std::unordered_map<GraphNode*, Node*> NodeMap;
		std::unordered_map<IEdge<EdgeData>*, Edge*> EdgeMap;
		AIngine::Events::Event<void> GraphLoadedEvent;

	private:
		void LoadGraph();
		void SpawnNode(GraphNode& node);
		void SpawnEdge(GraphNode& from, GraphNode& to);

	private:
		IGraph<NodeData, EdgeData>* m_graph;
		glm::vec2 min;
		glm::vec2 max;
		glm::vec2 scale;

	};

	template<typename N, typename E>
	bool Search(IGraphNode<N, E>* startNode, Task& task)
	{
		struct SearchValues {
			float gScore;
			float fScore;
		};

		task.Cost = std::numeric_limits<float>::max();

		// keep track of the nodes we haven't visited yet
		std::vector<IGraphNode<N, E>*> openSet;

		// keep track of the visited nodes
		std::unordered_set<IGraphNode<N, E>*> closedSet;

		// Map of nodes to keep track of the path taken
		std::unordered_map<IGraphNode<N, E>*, IGraphNode<N, E>*> cameFrom;

		// map of nodes and their search values used in A*
		std::unordered_map<IGraphNode<N, E>*, SearchValues> searchValueSet;

		openSet.push_back(startNode);
		searchValueSet[startNode] = SearchValues{ 0, task.Heuristic(startNode) };

		while (openSet.size() > 0) {

			// sort the openSet by the nodes' fScores
			std::sort(openSet.begin(), openSet.end(), [&searchValueSet](IGraphNode<N, E>* v, IGraphNode<N, E> * u) -> bool
			{ return searchValueSet[v].fScore < searchValueSet[u].fScore; });

			// current node is the node with the lowest fScore
			IGraphNode<NodeData, EdgeData>* current = openSet[0];

			// check if goal was reached yet and construct path if true
			if (task.GoalTest(current))
			{
				ReconstructPath(cameFrom, current, task.Path, &task.Cost);
				return true;
			}

			// node wasn't goal, remove if from the open set and add it to the closed set
			openSet.erase(openSet.begin());
			closedSet.emplace(current);

			for (auto& neighbor : current->m_neighbors) {

				if (closedSet.find(neighbor.first) != closedSet.end()) {
					continue; // ignore neighbour which is already evaluated
				}

				if (searchValueSet.find(neighbor.first) == searchValueSet.end())
				{
					openSet.push_back(neighbor.first); // discover a new node
					searchValueSet[neighbor.first] = SearchValues{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
				}

				// the new gScore is the distance traveled so far + the distance to the neighbor
				float gScore_temp = searchValueSet[current].gScore + current->GetEdge(neighbor.first)->Cost;

				// no pathcost improvement, skip 
				if (gScore_temp >= searchValueSet[neighbor.first].gScore) continue;

				// keep track of this step
				cameFrom[neighbor.first] = current;
				// set new values for neighbour's gScore and fScore
				searchValueSet[neighbor.first] = SearchValues{ gScore_temp, gScore_temp + task.Heuristic(neighbor.first) };
			}
		}

		return false;
	}

	template<class N, class E>
	void ReconstructPath(std::unordered_map<IGraphNode<N, E>*, IGraphNode<N, E>*>& cameFrom, IGraphNode<N, E>* current, std::vector<IGraphNode<N, E>*>& path, float* cost)
	{
		*cost = 0;
		path.clear();

		IGraphNode<N, E>* next = current;

		while (cameFrom.find(next) != cameFrom.end()) {
			*cost += cameFrom[next]->GetEdge(next)->Cost;
			path.push_back(next);
			next = cameFrom[next];
		}

		std::reverse(path.begin(), path.end());
	}
}