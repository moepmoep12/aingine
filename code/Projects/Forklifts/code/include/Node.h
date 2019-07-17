#pragma once
#include "AIngine/Core.h"
#include "Edge.h"

#include "nlohmann/json.hpp"

namespace Forklifts {

	template<class T>
	class INode {
	public:
		INode() {}
		INode(T data) : m_data(data) {}

		inline const T& Data() const { return m_data; }
		inline void SetData(T value) { m_data = value; }

	private:
		T m_data;
	};

	template<class T, class E>
	class IGraphNode : public INode<T> {
	public:
		inline void AddEdge(INode<T>* neighbor, IEdge<E>* edge) {
			if (neighbor == this) return;
			if (!neighbor) return;

			if (m_neighbors.find(neighbor) == m_neighbors.end())
				m_neighbors[neighbor] = edge;
		}

		inline IEdge<E>* AddEdge(INode<T>* neighbor, E& edgeData, float cost) {
			if (neighbor == this) return nullptr;
			if (!neighbor) return nullptr;

			IEdge<E>* edge = new IEdge<E>(edgeData, cost);
			AddEdge(neighbor, edge);
			return edge;
		}

		inline void ChangeWeight(INode<T>* neigbor, float cost) {
			if (neighbor == this) return;
			if (!neighbor) return;

			if (m_neighbors.find(neighbor) != m_neighbors.end())
				m_neighbors[neighbor].Cost = cost;
		}

		inline IEdge<E>* GetEdge(INode<T>* neighbor) {
			if (neighbor == this) return nullptr;
			if (!neighbor) return nullptr;

			if (m_neighbors.find(neighbor) != m_neighbors.end())
				return m_neighbors[neighbor];
			return nullptr;
		}

		//inline IEdge<E>* GetEdge(const IGraphNode<T, E>& neighbor) {
		//	if (&neighbor == this) return nullptr;

		//	if (m_neighbors.find(&neighbor) != m_neighbors.end())
		//		return m_neighbors[&neighbor];
		//	return nullptr;
		//}

		inline IEdge<E>* GetEdge(const T& nodeData) {
			for (auto& it = m_neighbors.begin(); it != m_neighbors.end(); it++) {
				if (it->first->Data() == nodeData)
					return it->first;
			}

			return nullptr;
		}


		std::map<INode<T>*, IEdge<E>*> m_neighbors;

	};

	struct NodeData {
		std::string Areal;
		std::string HasPlaces;
		std::string Name;
		std::string Bereich;
		float KoordX;
		float KoordY;
	};

	inline void to_json(nlohmann::json& j, const NodeData& data)
	{
		j = nlohmann::json{
			{"Areal", data.Areal},
			{"HasPlaces",data.HasPlaces},
			{"Knoten", data.Name},
			{"Bereich", data.Bereich},
			{"KoordX", data.KoordX},
			{"KoordY",data.KoordY}
		};
	}

	inline void from_json(const nlohmann::json& j, NodeData& data) {
		j.at("Areal").get_to(data.Areal);
		j.at("HasPlaces").get_to(data.HasPlaces);
		j.at("Knoten").get_to(data.Name);
		j.at("Bereich").get_to(data.Bereich);
		j.at("KoordX").get_to(data.KoordX);
		j.at("KoordY").get_to(data.KoordY);
	}

	class Node : public IGraphNode<NodeData, int>, public Script {
	public:
		const IGraphNode<NodeData, EdgeData>* node;
	};
}