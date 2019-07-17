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

	// End is called when gameplay ends for this script
	void Graph::OnEnd()
	{
		delete m_graph;
		m_graph = nullptr;
		NodeMap.clear();
		EdgeMap.clear();
	}

	// Update is called once per frame
	void Graph::Update(float delta)
	{
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

			if (data.KoordX < min.x)
				min.x = data.KoordX;
			if (data.KoordX > max.x)
				max.x = data.KoordX;

			if (data.KoordY < min.y)
				min.y = data.KoordY;
			if (data.KoordY > max.y)
				max.y = data.KoordY;
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

			cost = glm::distance(glm::vec2(toNode->Data().KoordX, toNode->Data().KoordY), glm::vec2(fromNode->Data().KoordX, fromNode->Data().KoordY));

			m_graph->AddEdgeDirectional(fromNode, toNode, new IEdge<EdgeData>(data, cost));

			if (data.Directed.empty())
				m_graph->AddEdgeDirectional(toNode, fromNode, new IEdge<EdgeData>(data, cost));

			SpawnEdge(*fromNode, *toNode);
		}

		for (auto& node : m_graph->nodes)
			SpawnNode(*node);

		GraphLoadedEvent();
	}

	void Graph::SpawnNode(GraphNode& node)
	{
		glm::vec2 pos(node.Data().KoordX, node.Data().KoordY);
		pos *= scale;

		GameObject* spawned = AIngine::World::SpawnObject(node.Data().Name, this->m_owner, pos);
		Sprite* sprite = spawned->AddComponent<Sprite>();
		sprite->SetTexture(Texture2D(AIngine::Assets::AssetRegistry::Load<BitmapAsset>("AIngine/textures/Circle.png")->GetBitmap()));
		sprite->SetLocalWorldSize(glm::vec2(0.075, 0.075));
		PhysicsComponent* physComp = spawned->AddComponent<PhysicsComponent>();
		AIngine::Physics::PhysicsProperties properties;
		physComp->CreateCircleBody(properties, AIngine::Physics::PhysicsBodyType::e_Static, 0.075 / 2.0, true, true);

		Node* n = spawned->AddComponent<Node>();
		n->node = &node;
		NodeMap[&node] = n;

	}

	void Graph::SpawnEdge(GraphNode& from, GraphNode& to)
	{
		static const float edgeHeight = 0.05f;

		glm::vec2 posFrom(from.Data().KoordX, from.Data().KoordY);
		posFrom *= scale;

		glm::vec2 posTo(to.Data().KoordX, to.Data().KoordY);
		posTo *= scale;

		float distance = glm::distance(posFrom, posTo);

		glm::vec2 pos(posFrom + glm::normalize(posTo - posFrom) * 0.5f* distance);

		std::stringstream name;
		name << "(" << from.Data().Name << " | " << to.Data().Name << ")";

		GameObject* spawned = AIngine::World::SpawnObject(name.str(), this->m_owner, pos);
		Sprite* sprite = spawned->AddComponent<Sprite>();
		sprite->SetTexture(Texture2D(AIngine::Assets::AssetRegistry::Load<BitmapAsset>("AIngine/textures/White.png")->GetBitmap()));
		sprite->SetLocalWorldSize(glm::vec2(distance, edgeHeight));
		sprite->SetColor(glm::vec4(0.24, 0.7, 1, 1));
		spawned->SetRotation(glm::acos(glm::dot(glm::normalize(posTo - posFrom), glm::vec2(1, 0))));

		Edge* e = spawned->AddComponent<Edge>();
		e->edge = from.GetEdge(&to);

		EdgeMap[from.GetEdge(&to)] = e;
	}
}