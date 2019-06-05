#include "Editor/Widgets/SceneGraphWidget.h"
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
#include "Debug/log.h"

namespace AIngine::Editor {

	static GameObject* s_selectedNode = nullptr;
	static GameObject* s_DropTarget = nullptr;

	void AIngine::Editor::SceneGraphWidget::OnImGuiRender()
	{
		static bool p_open = true;
		static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoMove*/;

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

	SceneGraphWidget::~SceneGraphWidget()
	{
		delete m_textureCompWidget;
		delete m_transformCompWidget;
	}


	SceneGraphWidget::SceneGraphWidget(AIngine::Structures::SceneGraph & sceneGraph)
		: m_sceneGraph(sceneGraph)
	{
		m_textureCompWidget = new TextureComponentWidget();
		m_transformCompWidget = new TransformComponentWidget();
	}

	void SceneGraphWidget::ShowSelectedNodeWidget(GameObject * node)
	{
		if (!node) return;

		m_transformCompWidget->Render(std::vector<GameObject*> { node });

		AIngine::Rendering::Texture2D* texture = node->GetComponent<AIngine::Rendering::Texture2D>();
		// show textureComponent
		if (texture) {
			m_textureCompWidget->Render(std::vector<GameObject*> { node });
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

		PerformDrop();

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

			BeginDragSource();
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

			BeginDragSource();

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

		// Drag & Drop
		BeginDragSource();
		BeginDropTarget(node);

		return true;
	}

	void SceneGraphWidget::ImguiTreeTraverser::PerformDrop()
	{
		if (s_DropTarget && s_selectedNode) {
			GameObject* dragSource = s_selectedNode;

			// we want our source to have the same WorldTransfrom after changing the parent
			// ==> we have to adjust the local transform according to the new parent
			glm::vec2 preWorldPos = dragSource->GetWorldPosition();
			glm::vec2 preWorldScale = dragSource->GetWorldScale();
			float preWorldRot = dragSource->GetWorldRotation();

			// first, remove the source as child from its parent
			dragSource->GetParent()->RemoveChild(dragSource);
			s_DropTarget->AddChild(dragSource);

			glm::vec2 postWorldPos = dragSource->GetWorldPosition();
			glm::vec2 postWorldScale = dragSource->GetWorldScale();
			float postWorldRot = dragSource->GetWorldRotation();

			dragSource->UpdateTransform(-(postWorldPos - preWorldPos), -(postWorldScale - preWorldScale), -(postWorldRot - preWorldRot));

			s_DropTarget = nullptr;
		}
	}

	void SceneGraphWidget::ImguiTreeTraverser::BeginDragSource()
	{
		ImGuiDragDropFlags target_flags = 0;
		target_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
		if (ImGui::BeginDragDropSource(target_flags)) {
			ImGui::SetDragDropPayload("SelectedNode", s_selectedNode, sizeof(GameObject));
			ImGui::EndDragDropSource();
		}
	}

	void SceneGraphWidget::ImguiTreeTraverser::BeginDropTarget(GameObject& obj)
	{
		// Begin Drop. Remember our target to use it AFTER iterating
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SelectedNode")) {
				IM_ASSERT(payload->DataSize == sizeof(GameObject));
				GameObject* source = s_selectedNode;
				if (source) {
					s_DropTarget = &obj;
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}

