#pragma once
#include "AIngine/Core.h"

#include "nlohmann/json.hpp"

namespace Forklifts {

	template<class T>
	struct IEdge {
		IEdge(T data, float cost) : Data(data), Cost(cost) {}
		T Data;
		float Cost = std::numeric_limits<float>::max();
	};

	struct EdgeData {
		std::string Areal;
		std::string StartKnoten;
		std::string EndKnoten;
		std::string Directed;
	};

	inline void to_json(nlohmann::json& j, const EdgeData& data)
	{
		j = nlohmann::json{
			{"Areal", data.Areal},
			{"StartKnoten",data.StartKnoten},
			{"EndKnoten", data.EndKnoten},
			{"Directed", data.Directed},
		};
	}

	inline void from_json(const nlohmann::json& j, EdgeData& data) {
		j.at("Areal").get_to(data.Areal);
		j.at("KnotenVon").get_to(data.StartKnoten);
		j.at("KnotenNach").get_to(data.EndKnoten);
		j.at("Gerichtet").get_to(data.Directed);
	}

	class Edge : public AIngine::Script {
	public:
		Edge();
		// Inherited via Script
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void Update(float delta) override;
		virtual void OnEventData(AIngine::Events::EventData& e) override;

		const IEdge<EdgeData>* edge;
	};
}