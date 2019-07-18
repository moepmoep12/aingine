#include "Graph.h"
#include "AIngine/Util/Project.h"
#include "Forklifts.h"

#include <fstream>
#include "nlohmann/json.hpp"
namespace Forklifts {

	// Constructor
	Graph::Graph()
		: min(std::numeric_limits<float>::max()), max(0), scale(1)
	{
		// In order for the editor to display the scripts name correctly
		SetName(typeid(*this).name());
		m_graph = new IGraph<NodeData, EdgeData>();
	}

	Graph::~Graph()
	{
		if (m_graph)
			delete m_graph;
	}

	// Start is called when gameplay starts for this script
	void Graph::OnStart()
	{
		LoadGraph();
	}

	static bool firstFrame = true;

	// End is called when gameplay ends for this script
	void Graph::OnEnd()
	{
		delete m_graph;
		m_graph = nullptr;
		NodeMap.clear();
		EdgeMap.clear();
		firstFrame = true;
	}


	// Update is called once per frame
	void Graph::Update(float delta)
	{
		if (firstFrame) {
			GraphLoadedEvent();
			firstFrame = false;
		}
	}

	// Callback for events
	void Graph::OnEventData(AIngine::Events::EventData & e)
	{
		// key pressed
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {
			AIngine::Events::KeyPressedEvent::KeyPressedEventData pressedEvent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);

			if (pressedEvent.GetKeyCode() == AIngine::KeyCode::R) {
				Forklifts::Get().bRender = !Forklifts::Get().bRender;
			}
		}
	}

	GraphNode * Graph::FindNode(const std::string & name)
	{
		auto found = std::find_if(m_graph->nodes.begin(), m_graph->nodes.end(), [=](const GraphNode* n) {
			return n->Data().Name == name;
		});
		if (found != m_graph->nodes.end())
			return *found._Ptr;
		return nullptr;
	}

	void Graph::LoadGraph()
	{
		using json = nlohmann::json;
		std::string path = AIngine::Util::Project::GetResourceDirectory() + nodeJsonPath;
		// open the file
		std::ifstream file;
		file.open(path);
		if (file.fail()) return;
		json j = json::parse(file);
		file.close();

		// Load Nodes
		for (auto val : j) {
			NodeData data = val;
			GraphNode* node = new GraphNode();
			node->SetData(data);
			m_graph->AddNode(node);

			if (data.Koords.x < min.x)
				min.x = data.Koords.x;
			if (data.Koords.x > max.x)
				max.x = data.Koords.x;

			if (data.Koords.y < min.y)
				min.y = data.Koords.x;
			if (data.Koords.y > max.y)
				max.y = data.Koords.y;
		}


		scale.x = 20 / (max.x - min.x);
		scale.y = 11.25 / (max.y - min.y);


		// Load Edges
		path = AIngine::Util::Project::GetResourceDirectory() + edgeJsonPath;
		file.open(path);
		if (file.fail()) return;
		j = json::parse(file);
		file.close();

		for (auto val : j) {
			GraphNode* fromNode = nullptr;
			GraphNode* toNode = nullptr;
			float cost = 0;
			EdgeData data = val;

			fromNode = FindNode(data.StartKnoten);
			toNode = FindNode(data.EndKnoten);

			if (!toNode || !fromNode) continue;

			cost = glm::distance(toNode->Data().Koords, fromNode->Data().Koords);

			m_graph->AddEdgeDirectional(fromNode, toNode, new IEdge<EdgeData>(data, cost));

			if (data.Directed.empty())
				m_graph->AddEdgeDirectional(toNode, fromNode, new IEdge<EdgeData>(data, cost));

			SpawnEdge(*fromNode, *toNode);
		}

		for (auto& node : m_graph->nodes)
			SpawnNode(*node);

	}

	void Graph::SpawnNode(GraphNode& node)
	{
		glm::vec2 pos(node.Data().Koords);
		pos *= scale;

		GameObject* spawned = AIngine::World::SpawnObject(node.Data().Name, this->m_owner, pos);
		Sprite* sprite = spawned->AddComponent<Sprite>();
		sprite->SetTexture(Texture2D(AIngine::Assets::AssetRegistry::Load<BitmapAsset>("AIngine/textures/Circle.png")->GetBitmap()));
		sprite->SetLocalWorldSize(glm::vec2(0.15, 0.15));
		sprite->SetColor(glm::vec4(0, 0.176, 1.0, 1.0));
		PhysicsComponent* physComp = spawned->AddComponent<PhysicsComponent>();
		AIngine::Physics::PhysicsProperties properties;
		physComp->CreateCircleBody(properties, AIngine::Physics::PhysicsBodyType::e_Static, 0.15 / 2.0, true, true);

		Node* n = spawned->AddComponent<Node>();
		n->node = &node;
		NodeMap[&node] = n;

	}

	void Graph::SpawnEdge(GraphNode& from, GraphNode& to)
	{
		static const float edgeHeight = 0.05f;

		glm::vec2 posFrom(from.Data().Koords);
		posFrom *= scale;

		glm::vec2 posTo(to.Data().Koords);
		posTo *= scale;

		float distance = glm::distance(posFrom, posTo);

		glm::vec2 pos(posFrom + glm::normalize(posTo - posFrom) * 0.5f* distance);

		std::stringstream name;
		name << "(" << from.Data().Name << " | " << to.Data().Name << ")";

		GameObject* spawned = AIngine::World::SpawnObject(name.str(), this->m_owner, pos);
		Sprite* sprite = spawned->AddComponent<Sprite>();
		sprite->SetTexture(Texture2D(AIngine::Assets::AssetRegistry::Load<BitmapAsset>("AIngine/textures/White.png")->GetBitmap()));
		sprite->SetLocalWorldSize(glm::vec2(distance, edgeHeight));
		sprite->SetColor(glm::vec4(0.955, 1.0, 0.24, 1));
		glm::vec2 v1 = glm::normalize(posTo - posFrom);
		glm::vec2 v2 = glm::vec2(1, 0);
		float rot = glm::atan(v1.y, v1.x) - glm::atan(v2.y, v2.x);
		spawned->SetRotation(rot);

		Edge* e = spawned->AddComponent<Edge>();
		e->edge = from.GetEdge(&to);

		EdgeMap[from.GetEdge(&to)] = e;
	}
}