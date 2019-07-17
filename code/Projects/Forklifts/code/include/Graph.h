#pragma once
#include "AIngine/Core.h"
#include "Edge.h"
#include "Node.h"

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

	private:
		void LoadGraph();
		void SpawnNode(const GraphNode& node);
		void SpawnEdge(GraphNode& from, GraphNode& to);

	private:
		IGraph<NodeData, EdgeData>* m_graph;
		glm::vec2 min;
		glm::vec2 max;
		glm::vec2 scale;

	};
}