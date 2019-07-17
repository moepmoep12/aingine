#include "Experiment.h"
#include "Util/Project.h"

#include "nlohmann/json.hpp"
#include <fstream>

namespace Forklifts {

	// Constructor
	Experiment::Experiment()
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
	}

	// Start is called when gameplay starts for this script
	void Experiment::OnStart()
	{

		m_graph = GetChild("Graph")->GetComponent<Graph>();
		OnGraphLoadedEventHandler = AIngine::Events::EventHandler<void>(std::bind(&Experiment::Init, this));
		m_graph->GraphLoadedEvent += OnGraphLoadedEventHandler;

	}

	// End is called when gameplay ends for this script
	void Experiment::OnEnd()
	{
		m_forkLiftTasks.clear();
		m_graph->GraphLoadedEvent -= OnGraphLoadedEventHandler;
	}

	// Update is called once per frame
	void Experiment::Update(float delta)
	{
	}

	// Callback for events
	void Experiment::OnEventData(AIngine::Events::EventData & e)
	{
	}

	void Experiment::Init()
	{
		for (int i = 0; i < forkLiftCount; i++) {
			std::stringstream name;
			std::stringstream path;
			name << "Forklift" << i;
			Forklift* forklift = AIngine::World::GetGameObject(name.str())->GetComponent<Forklift>();
			m_forkLiftTasks.push_back({ forklift, std::vector<Task>() });
			path << AIngine::Util::Project::GetResourceDirectory() << "SFAs" << i << ".txt";

			using json = nlohmann::json;
			// open the file
			std::ifstream file;
			file.open(path.str());
			if (file.fail()) return;
			json j = json::parse(file);
			file.close();

			for (auto& val : j) {
				std::string from = val.at("KnotenVon");
				std::string to = val.at("KnotenNach");

				GraphNode* fromNode = m_graph->FindNode(from);
				GraphNode* toNode = m_graph->FindNode(to);

				Task task{
					m_graph->NodeMap[fromNode],
					m_graph->NodeMap[toNode]
				};

				m_forkLiftTasks[i].second.push_back(task);
			}

			forklift->GetOwner()->SetWorldPosition(m_forkLiftTasks[i].second[0].startNode->GetOwner()->GetWorldPosition());
		}
	}
}