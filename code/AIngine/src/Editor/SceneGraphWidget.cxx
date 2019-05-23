#include "Editor/SceneGraphWidget.h"
#include "AIngine/Constants.h"
#include "imgui.h"
#include "Structures/SceneGraph.h"
//#include "Application.h"
#include "AIngine/GameObject.h"
#include "Rendering/texture.h"
#include "AIngine/Input.h"
#include "AIngine/KeyCodes.h"
#include "Events/InputEvents.h"
#include "AIngine/Physics.h"

namespace AIngine::Editor {

	static GameObject* s_selectedNode = nullptr;

	void AIngine::Editor::SceneGraphWidget::OnImGuiRender()
	{
		static bool p_open = true;
		static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoMove*/;

		//ImGui::SetNextWindowSize(ImVec2((float)windowWidth, (float)windowHeight), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin("SceneGraph", &p_open, windowFlags))
		{
			ImGui::End();
			return;
		}

		m_size = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		glm::vec2 pos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		m_wasSizeChanged = pos != m_position;
		m_position = pos;
		m_isDocked = ImGui::IsWindowDocked();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		// traverse tree and create tree with imgui
		ImguiTreeTraverser traverser;
		traverser.Traverse(&m_sceneGraph.GetRoot());

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
		ShowSelectedNodeWidget(s_selectedNode);
		ImGui::End();
	}

	void SceneGraphWidget::OnEvent(AIngine::Events::Event & e)
	{
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent)) {
			// delete event
			AIngine::Events::KeyPressedEvent keyevent = dynamic_cast<AIngine::Events::KeyPressedEvent&>(e);
			if (s_selectedNode && keyevent.GetKeyCode() == AIngine::KeyCodes::DEL) {
				AIngine::Structures::DeleteTraverser deletetraverser(m_sceneGraph.m_gameObjectPool);
				deletetraverser.Traverse(s_selectedNode);
				s_selectedNode = nullptr;
			}
		}
	}


	SceneGraphWidget::SceneGraphWidget(AIngine::Structures::SceneGraph & sceneGraph)
		: m_sceneGraph(sceneGraph)
	{
	}

	void SceneGraphWidget::ShowSelectedNodeWidget(GameObject * node)
	{
		if (!node) return;

		float* position[] = { &node->GetLocalPosition().x ,&node->GetLocalPosition().y };
		float* scale[] = { &node->GetLocalScale().x , &node->GetLocalScale().y };
		ImGui::Text("Transform");
		bool draggedPosition = ImGui::DragFloat2("Position", *position);
		bool draggedScale = ImGui::DragFloat2("Scale", *scale);
		bool draggedRot = ImGui::DragFloat("Rotation", &node->GetLocalRotation(), M_PI / 180.0f);

		AIngine::Rendering::Texture2D* texture = node->GetComponent<AIngine::Rendering::Texture2D>();

		// show textureComponent
		if (texture) {
			ImGui::Separator();
			static float dragSpeed = 0.1f;
			float* size[] = { &texture->GetLocalWorldSize().x ,&texture->GetLocalWorldSize().y };
			float* color[] = { &texture->GetColor().x,&texture->GetColor().y ,&texture->GetColor().z };
			float* parallax[] = { &texture->GetParallaxFactor().x, &texture->GetParallaxFactor().y };
			ImGui::Text("Texture Component");
			ImGui::SameLine();
			ImGui::Text(texture->GetName().c_str());
			ImGui::DragFloat2("WorldSize", *size, dragSpeed, 0.0f, 1000.0f);
			ImGui::DragFloat3("Color", *color, 0.02f, 0.0f, 1.0f);
			ImGui::DragFloat2("Parallax Factor", *parallax, 0.02f, -100.0f, 100.0f);
		}

		// adjust position & rotation in the phyiscs world
		AIngine::PhysicsComponent* physComp = node->GetComponent<AIngine::PhysicsComponent>();
		if (physComp) {
			b2Vec2 worldPos = b2Vec2(node->GetWorldPosition().x, node->GetWorldPosition().y);
			if (draggedPosition || draggedRot)
				physComp->m_body->SetTransform(worldPos, node->GetWorldRotation());
		}
	}

	/*------------------------------------------- IMGUI TREE TRAVERSER -----------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------------------*/


	SceneGraphWidget::ImguiTreeTraverser::ImguiTreeTraverser()
	{

	}

	SceneGraphWidget::ImguiTreeTraverser::~ImguiTreeTraverser()
	{
		m_openNodesMap.clear();
	}

	bool SceneGraphWidget::ImguiTreeTraverser::Traverse(GameObject * root)
	{
		// root is always open
		ImGui::SetNextTreeNodeOpen(true);

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 2); // Increase spacing to differentiate leaves from expanded contents.
		bool result = root->Accept(*this);
		ImGui::PopStyleVar();
		return result;
	}

	bool SceneGraphWidget::ImguiTreeTraverser::Enter(GameObject & node)
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

	bool SceneGraphWidget::ImguiTreeTraverser::Leave(GameObject & node)
	{
		if (m_openNodesMap[&node]) {
			ImGui::TreePop();
		}
		return true;
	}

	// Creates a leaf
	bool SceneGraphWidget::ImguiTreeTraverser::Visit(GameObject & node)
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

