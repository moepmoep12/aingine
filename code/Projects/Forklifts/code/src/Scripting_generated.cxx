#include "AIngine/Core.h"
#include "AIngine/GameObject.h"
#include "Node.h"
#include "Edge.h"
#include "Graph.h"
#include "Forklift.h"
#include "Experiment.h"
#include "ForkliftAgent.h"
#include <vector>
#include <string>

std::vector<std::string> AIngine::ApplicationComponentNames = {
    "Node",
    "Edge",
    "Graph",
    "Forklift",
    "Experiment",
    "ForkliftAgent",
};

void AIngine::OnAddComponent(AIngine::GameObject* obj, int index) {
	switch (index) {
	case 0:
		obj->AddComponent<Forklifts::Node>()->ScriptIndex = 0;
		break;

	case 1:
		obj->AddComponent<Forklifts::Edge>()->ScriptIndex = 1;
		break;

	case 2:
		obj->AddComponent<Forklifts::Graph>()->ScriptIndex = 2;
		break;

	case 3:
		obj->AddComponent<Forklifts::Forklift>()->ScriptIndex = 3;
		break;

	case 4:
		obj->AddComponent<Forklifts::Experiment>()->ScriptIndex = 4;
		break;

	case 5:
		obj->AddComponent<Forklifts::ForkliftAgent>()->ScriptIndex = 5;
		break;

	}
}