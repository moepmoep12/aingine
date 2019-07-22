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
		TaskFinishedHandler = Forklift::OnTaskFinishedEventHandler(std::bind(&Experiment::OnForkliftTaskFinished, this, std::placeholders::_1));

		AmountCollisions = 0;
	}

	// End is called when gameplay ends for this script
	void Experiment::OnEnd()
	{
		for (auto& pair : m_forkLiftTasks) {
			pair.first->OnTaskFinishedEvent = AIngine::Events::Event<void, Forklift*>();
		}
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

	void Experiment::OnGUI()
	{
		std::stringstream text;
		text << "Collisions : " << AmountCollisions;
		AIngine::Graphics::Text(text.str(), glm::vec2(10, 100));
	}

	void Experiment::Init()
	{
		for (int i = 0; i < forkLiftCount; i++) {
			std::stringstream name;
			std::stringstream path;
			name << "Forklift" << i;
			auto comps = AIngine::World::GetGameObject(name.str())->GetComponents();
			Forklift* forklift = nullptr;
			for (auto&& comp : comps) {
				if (dynamic_cast<Forklift*>(comp)) {
					forklift = dynamic_cast<Forklift*>(comp);
				}
			}
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

				Search<NodeData, EdgeData>(fromNode, task);

				m_forkLiftTasks[i].second.push_back(task);
			}

			size_t size = m_forkLiftTasks[i].second.size();
			size_t inserted = 0;
			for (int j = 0; j < size - 1; j++) {
				Task& current = m_forkLiftTasks[i].second[j + inserted];
				Task& next = m_forkLiftTasks[i].second[j + 1 + inserted];

				// Leerfahrt
				if (current.endNode->node != next.startNode->node) {
					Task task{
						m_graph->NodeMap[current.endNode->node],
						m_graph->NodeMap[next.startNode->node]
					};

					Search<NodeData, EdgeData>(current.endNode->node, task);
					m_forkLiftTasks[i].second.insert(m_forkLiftTasks[i].second.begin() + j + 1 + inserted, task);
					inserted++;
				}
			}

			forklift->GetOwner()->SetWorldPosition(m_forkLiftTasks[i].second[0].startNode->GetOwner()->GetWorldPosition());
			forklift->_currentNode = m_forkLiftTasks[i].second[0].startNode->node;
			forklift->OnTaskFinishedEvent += TaskFinishedHandler;
			forklift->ReceiveTask(&m_forkLiftTasks[i].second[0]);
		}
	}

	void Experiment::OnForkliftTaskFinished(Forklift * forklift)
	{
		forklift->currentTaskIndex++;
		auto result = std::find_if(m_forkLiftTasks.begin(), m_forkLiftTasks.end(), [=](const std::pair<Forklift*, std::vector<Task>>& pair) -> bool
		{
			return pair.first == forklift;
		});

		if (result != m_forkLiftTasks.end()) {
			if (forklift->currentTaskIndex < (*result._Ptr).second.size()) {
				forklift->ReceiveTask(&(*result._Ptr).second[forklift->currentTaskIndex]);
			}
		}

	}
}