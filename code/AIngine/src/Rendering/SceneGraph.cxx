#include "Rendering/SceneGraph.h"
#include <sstream>
#include "log.h"
#include "imgui.h"
#include "Application.h"

namespace AIngine::Rendering {

	static unsigned int nodeCount = 1;

	SceneGraph::SceneGraph() : Layer("SceneGraph Layer")
	{
		m_Root = new (m_GroupNodePool.Allocate()) GroupNode(nullptr, "Root");
	}

	SceneGraph::~SceneGraph()
	{
		DeleteTraverser deleteTreeTraverser(m_Root, m_shapeNodePool, m_GroupNodePool);
	}


	ShapeNode* SceneGraph::AddShape(Texture2D * texture, GroupNode * parent, const glm::vec2 & position, const glm::vec2 & scale, const float rotation)
	{
		static std::string name("ShapeNode");
		std::stringstream stream;
		stream << name << nodeCount;
		if (!parent) {
			parent = m_Root;
		}

		ShapeNode* newNode = new(m_shapeNodePool.Allocate()) ShapeNode(texture, parent, stream.str(), position, scale, rotation);
		parent->AddChild(newNode);
		nodeCount++;
		return newNode;
	}
	GroupNode * SceneGraph::AddGroup(GroupNode * parent, const glm::vec2 & position, const glm::vec2 & scale, const float rotation)
	{
		static std::string name("GroupNode");
		std::stringstream stream;
		stream << name << nodeCount;

		if (!parent) {
			parent = m_Root;
		}

		GroupNode* newNode = new (m_GroupNodePool.Allocate()) GroupNode(parent, stream.str(), position, scale, rotation);
		parent->AddChild(newNode);
		nodeCount++;
		return newNode;
	}

	bool SceneGraph::RemoveNode(SceneNode * node)
	{
		DeleteTraverser deleteTraverser(node, m_shapeNodePool, m_GroupNodePool);
		return true;
	}

	static SceneNode* s_selectedNode = nullptr;

	void SceneGraph::OnImGuiRender()
	{
		static unsigned int windowWidth = 400;
		unsigned int windowHeight = AIngine::Application::Get().GetWindow().GetHeight();
		static bool p_open = true;
		static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

		ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin("SceneGraph", &p_open, windowFlags))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		// traverse tree and create tree with imgui
		ImguiTreeTraverser traverser(m_Root);
		traverser.Traverse();

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
		ShowSelectedNodeWidget(s_selectedNode);
		ImGui::End();

	}


	void SceneGraph::ShowSelectedNodeWidget(SceneNode * node)
	{
		if (!node) return;
		ImGui::InputFloat("Position X", &node->GetPosition().x);
		ImGui::InputFloat("Position Y", &node->GetPosition().y);
		ImGui::InputFloat("Rotation", &node->GetRotation());
		ImGui::InputFloat("Scale X", &node->GetScale().x);
		ImGui::InputFloat("Scale Y", &node->GetScale().y);
	}

	/********************************** IMGUI TREE TRAVERSER ****************************************/


	ImguiTreeTraverser::ImguiTreeTraverser(SceneNode * root) : m_root(root)
	{

	}

	ImguiTreeTraverser::~ImguiTreeTraverser()
	{
		m_openNodesMap.clear();
	}


	bool ImguiTreeTraverser::Enter(GroupNode & node)
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		
		// selected
		if (s_selectedNode == &node)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		// create a inner node with children
		if (node.GetChildren().size() > 0)
		{
			bool node_open = ImGui::TreeNodeEx(&node, node_flags, node.GetName().c_str());

			// check if selected
			if (ImGui::IsItemClicked()) {
				s_selectedNode = &node;
			}

			m_openNodesMap[&node] = node_open;

			return node_open;
		}

		// create a leaf
		else {
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGui::TreeNodeEx(&node, node_flags, node.GetName().c_str());
			if (ImGui::IsItemClicked()) {
				s_selectedNode = &node;
			}
			return false;
		}

	}

	bool ImguiTreeTraverser::Leave(GroupNode & node)
	{
		if (m_openNodesMap[&node]) {
			ImGui::TreePop();
		}
		return true;
	}

	// Creates a leaf
	bool ImguiTreeTraverser::Visit(ShapeNode & node)
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

				// selected
		if (s_selectedNode == &node)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::TreeNodeEx(&node, node_flags, node.GetName().c_str());
		if (ImGui::IsItemClicked()) {
			s_selectedNode = &node;
		}

		return true;
	}

	void ImguiTreeTraverser::Traverse()
	{
		// root is always open
		ImGui::SetNextTreeNodeOpen(true);

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 2); // Increase spacing to differentiate leaves from expanded contents.
		m_root->Accept(*this);
		ImGui::PopStyleVar();
	}


	/********************************** DELETE TRAVERSER ****************************************/

	DeleteTraverser::DeleteTraverser(SceneNode * node, AIngine::Memory::Pool<ShapeNode>& shapeNodePool, AIngine::Memory::Pool<GroupNode>& GroupNodePool)
		: m_shapeNodePool(&shapeNodePool), m_GroupNodePool(&GroupNodePool)
	{


		GroupNode* parent = dynamic_cast<GroupNode*>(&node->GetParent());
		if (parent) {
			parent->RemoveChild(node);
		}

		node->Accept(*this);

		nodeCount -= (m_shapeNodesToDelete.size() + m_groupNodesToDelete.size());

		for (auto it = m_shapeNodesToDelete.begin(); it < m_shapeNodesToDelete.end(); it++) {
			m_shapeNodePool->Free(*it);
			(*it)->~ShapeNode();
		}

		for (auto it = m_groupNodesToDelete.begin(); it < m_groupNodesToDelete.end(); it++) {
			m_GroupNodePool->Free(*it);
			(*it)->~GroupNode();
		}

		CORE_INFO("Deleted {0} nodes. NodeCount is now at {1}", m_shapeNodesToDelete.size() + m_groupNodesToDelete.size(), nodeCount);

	}

	DeleteTraverser::~DeleteTraverser()
	{
		m_groupNodesToDelete.clear();
		m_shapeNodesToDelete.clear();
	}

	bool DeleteTraverser::Enter(GroupNode & node)
	{
		m_groupNodesToDelete.push_back(&node);
		return true;
	}
	bool DeleteTraverser::Leave(GroupNode & node)
	{
		return true;
	}

	bool DeleteTraverser::Visit(ShapeNode & node)
	{
		m_shapeNodesToDelete.push_back(&node);
		return true;
	}

}