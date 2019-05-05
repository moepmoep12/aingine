#include "SceneGraph.h"
#include <sstream>
#include "log.h"
#include "imgui.h"
#include "Application.h"
#include "Component.h"
#include "GameObject.h"


namespace AIngine {

	static unsigned int nodeCount = 1;

	SceneGraph::SceneGraph() : Layer("SceneGraph Layer")
	{
		m_Root = new (m_gameObjectPool.Allocate()) GameObject(nullptr, std::string("Root"));
	}

	SceneGraph::~SceneGraph()
	{
		DeleteTraverser deleteTreeTraverser(m_gameObjectPool);
		deleteTreeTraverser.Traverse(m_Root);
	}

	GameObject * const SceneGraph::SpawnObject(const std::string & name, GameObject * parent, const glm::vec2 & position, const glm::vec2 & scale, const float rotation)
	{
		if (!parent) {
			parent = m_Root;
		}

		GameObject* newObject = new (m_gameObjectPool.Allocate()) GameObject(parent, name);
		newObject->SetLocalPosition(position);
		newObject->SetLocalScale(scale);
		newObject->SetRotation(rotation);

		parent->AddChild(newObject);
		return newObject;
	}

	void SceneGraph::DestroyObject(GameObject & gameobject)
	{
		DeleteTraverser deleteTraverser(m_gameObjectPool);
		deleteTraverser.Traverse(&gameobject);
	}

	//bool SceneGraph::RemoveNode(SceneNode * node)
	//{
	//	DeleteTraverser deleteTraverser(node, m_gameObjectPool, m_GroupNodePool);
	//	return true;
	//}

	static GameObject* s_selectedNode = nullptr;

	void SceneGraph::OnUpdate(float delta)
	{
		UpdateTraverser updateTraverser(delta);
		updateTraverser.Traverse(m_Root);
	}

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
		AIngine::UI::ImguiTreeTraverser traverser;
		traverser.Traverse(m_Root);

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
		ShowSelectedNodeWidget(s_selectedNode);
		ImGui::End();

	}


	void SceneGraph::ShowSelectedNodeWidget(GameObject * node)
	{
		if (!node) return;
		ImGui::InputFloat("Position X", &node->GetLocalPosition().x);
		ImGui::InputFloat("Position Y", &node->GetLocalPosition().y);
		ImGui::InputFloat("Rotation", &node->GetLocalRotation());
		ImGui::InputFloat("Scale X", &node->GetLocalScale().x);
		ImGui::InputFloat("Scale Y", &node->GetLocalScale().y);
	}

	/********************************** IMGUI TREE TRAVERSER ****************************************/

	namespace UI {

		ImguiTreeTraverser::ImguiTreeTraverser()
		{

		}

		ImguiTreeTraverser::~ImguiTreeTraverser()
		{
			m_openNodesMap.clear();
		}

		bool ImguiTreeTraverser::Traverse(GameObject * root)
		{
			// root is always open
			ImGui::SetNextTreeNodeOpen(true);

			ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 2); // Increase spacing to differentiate leaves from expanded contents.
			bool result = root->Accept(*this);
			ImGui::PopStyleVar();
			return result;
		}

		bool ImguiTreeTraverser::Enter(GameObject & node)
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

		bool ImguiTreeTraverser::Leave(GameObject & node)
		{
			if (m_openNodesMap[&node]) {
				ImGui::TreePop();
			}
			return true;
		}

		// Creates a leaf
		bool ImguiTreeTraverser::Visit(GameObject & node)
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
	}
	/********************************** DELETE TRAVERSER ****************************************/

	DeleteTraverser::DeleteTraverser(AIngine::Memory::Pool<GameObject>& gameObjectPool)
		: m_gameObjectPool(&gameObjectPool)
	{

	}

	DeleteTraverser::~DeleteTraverser()
	{
		m_gameObjectsToDelete.clear();
	}

	bool DeleteTraverser::Traverse(GameObject* root)
	{
		GameObject* parent = &root->GetParent();

		if (parent) {
			parent->RemoveChild(root);
		}

		bool result = root->Accept(*this);

		for (auto it = m_gameObjectsToDelete.begin(); it < m_gameObjectsToDelete.end(); it++) {
			m_gameObjectPool->Free(*it);
			(*it)->~GameObject();
		}

		return result;
	}

	bool DeleteTraverser::Enter(GameObject & node)
	{
		m_gameObjectsToDelete.push_back(&node);
		return true;
	}
	bool DeleteTraverser::Leave(GameObject & node)
	{
		return true;
	}

	bool DeleteTraverser::Visit(GameObject & node)
	{
		m_gameObjectsToDelete.push_back(&node);
		return true;
	}

	/********************************** UPDATE TRAVERSER ****************************************/


	UpdateTraverser::UpdateTraverser(float deltaTime) : m_deltaTime(deltaTime)
	{
	}

	UpdateTraverser::~UpdateTraverser()
	{
	}

	bool UpdateTraverser::Traverse(GameObject * root)
	{
		return	root->Accept(*this);
	}

	bool UpdateTraverser::Enter(GameObject & node)
	{
		auto it = node.GetComponents().begin();

		while (it != node.GetComponents().end()) {
			Component* comp = *it._Ptr;
			if (comp->IsActive()) {
				comp->OnUpdate(m_deltaTime);
			}
			it++;
		}
		return true;
	}

	bool UpdateTraverser::Leave(GameObject & node)
	{
		return true;
	}

	bool UpdateTraverser::Visit(GameObject & node)
	{
		auto it = node.GetComponents().begin();

		while (it != node.GetComponents().end()) {
			Component* comp = *it._Ptr;
			if (comp->IsActive()) {
				comp->OnUpdate(m_deltaTime);
			}
			it++;
		}
		return true;
	}

}