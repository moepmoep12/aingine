#include "Editor/Widgets/SceneGraphWidget.h"
#include "AIngine/Constants.h"
#include "imgui.h"
#include "Structures/SceneGraph.h"
#include "AIngine/GameObject.h"
#include "Rendering/texture.h"
#include "AIngine/Input.h"
#include "AIngine/KeyCodes.h"
#include "Events/InputEvents.h"
#include "AIngine/Physics.h"
#include "Debug/log.h"
#include "Editor/Editor.h"
#include "AIngine/Sprite.h"
#include "AIngine/World.h"
#include "AIngine/SoundComponent.h"
#include "AIngine/Script.h"
#include "AIngine/ParticleEmitter.h"
#include "UI/Button.h"
#include "UI/Image.h"
#include "UI/Text.h"
#include "UI/CheckBox.h"
#include "UI/Slider.h"


namespace AIngine::Editor::Widget {

	static GameObject* s_selectedNode = nullptr;
	static GameObject* s_DropTarget = nullptr;
	static GameObject* s_CopySource = nullptr;

	void SceneGraphWidget::OnImGuiRender()
	{
		static bool p_open = true;
		static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoMove*/;

		if (!ImGui::Begin("SceneGraph", &p_open, windowFlags))
		{
			m_isDocked = false;
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

		PerformCopy();

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
		if (s_selectedNode) {
			ShowSelectedNodeWidget(s_selectedNode);
		}
		ImGui::End();
	}

	void SceneGraphWidget::OnEvent(AIngine::Events::EventData & e)
	{
		if (typeid(e) == typeid(AIngine::Events::KeyPressedEvent::KeyPressedEventData)) {
			// delete event
			AIngine::Events::KeyPressedEvent::KeyPressedEventData keyevent = dynamic_cast<AIngine::Events::KeyPressedEvent::KeyPressedEventData&>(e);
			if (s_selectedNode && keyevent.GetKeyCode() == AIngine::KeyCode::DEL) {
				AIngine::Structures::DeleteTraverser deletetraverser(m_sceneGraph.m_gameObjectPool);
				deletetraverser.Traverse(s_selectedNode);
				s_selectedNode = nullptr;
			}

			// duplicate
			if (s_selectedNode && keyevent.GetKeyCode() == AIngine::KeyCode::D && AIngine::Input::IsKeyPressed(AIngine::KeyCode::LEFT_CONTROL))
			{
				if (&m_sceneGraph.GetRoot() != s_selectedNode)
					s_selectedNode = &m_sceneGraph.Copy(*s_selectedNode);
			}
		}
	}

	SceneGraphWidget::~SceneGraphWidget()
	{
		delete m_textureCompWidget;
		delete m_transformCompWidget;
		delete m_physCompWidget;
		delete m_addComponentWidget;
		delete m_soundComponentWidget;
		delete m_particleEmitterWidget;
		delete m_buttonComponentWidget;
		delete m_canvasComponentWidget;
		delete m_imageComponentWidget;
		delete m_textComponentWidget;
		delete m_checkBoxComponentWidget;
		delete m_sliderComponentWidget;
	}


	SceneGraphWidget::SceneGraphWidget(AIngine::Structures::SceneGraph & sceneGraph)
		: m_sceneGraph(sceneGraph)
	{
		m_textureCompWidget = new Component::SpriteComponentWidget();
		m_transformCompWidget = new Component::TransformComponentWidget();
		m_physCompWidget = new Component::PhysicsComponentWidget();
		m_addComponentWidget = new Component::AddComponentWidget();
		m_soundComponentWidget = new Component::SoundComponentWidget();
		m_particleEmitterWidget = new Component::ParticleEmitterWidget();
		m_buttonComponentWidget = new Component::ButtonComponentWidget();
		m_canvasComponentWidget = new Component::CanvasComponentWidget();
		m_imageComponentWidget = new Component::ImageComponentWidget();
		m_textComponentWidget = new Component::UITextComponentWidget();
		m_checkBoxComponentWidget = new Component::CheckBoxComponentWidget();
		m_sliderComponentWidget = new Component::SliderComponentWidget();
	}

	void SceneGraphWidget::ShowSelectedNodeWidget(GameObject * node)
	{
		if (!node) return;

		m_transformCompWidget->Render(std::vector<GameObject*> { node });

		if (node != &m_sceneGraph.GetRoot()) {

			AIngine::Sprite* sprite = node->GetComponent<AIngine::Sprite>();
			// show textureComponent
			if (sprite) {
				m_textureCompWidget->Render(std::vector<GameObject*> { node });
			}

			AIngine::Physics::PhysicsComponent* physComp = node->GetComponent<AIngine::Physics::PhysicsComponent>();
			if (physComp) {
				m_physCompWidget->Render({ node });
			}

			AIngine::SoundComponent* soundComp = node->GetComponent<AIngine::SoundComponent>();
			if (soundComp) {
				m_soundComponentWidget->Render({ node });
			}

			AIngine::ParticleEmitter* emitter = node->GetComponent<AIngine::ParticleEmitter>();
			if (emitter) {
				m_particleEmitterWidget->Render({ node });
			}

			AIngine::UI::Canvas* canvas = node->GetComponent<AIngine::UI::Canvas>();
			if (canvas) {
				m_canvasComponentWidget->Render({ node });
			}

			AIngine::UI::Button* btn = node->GetComponent<AIngine::UI::Button>();
			if (btn) {
				m_buttonComponentWidget->Render({ node });
			}
			AIngine::UI::Image* img = node->GetComponent<AIngine::UI::Image>();
			if (img) {
				m_imageComponentWidget->Render({ node });
			}
			AIngine::UI::UIText* txt = node->GetComponent<AIngine::UI::UIText>();
			if (txt) {
				m_textComponentWidget->Render({ node });
			}
			AIngine::UI::CheckBox* checkbox = node->GetComponent<AIngine::UI::CheckBox>();
			if (checkbox) {
				m_checkBoxComponentWidget->Render({ node });
			}
			AIngine::UI::Slider* slider = node->GetComponent<AIngine::UI::Slider>();
			if (slider) {
				m_sliderComponentWidget->Render({ node });
			}

			ShowUserScripts(node);

			m_addComponentWidget->Render({ node });
		}
	}

	void SceneGraphWidget::ShowUserScripts(GameObject * node)
	{
		ImGui::Separator();
		// Title
		float textWidth = ImGui::CalcTextSize("User Scripts").x;
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) * 0.5f);
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "User Scripts");
		ImGui::NewLine();

		AIngine::Script* scriptToRemove = nullptr;

		for (auto& it = node->GetComponents().begin(); it != node->GetComponents().end(); it++) {
			AIngine::Script* script = dynamic_cast<AIngine::Script *>((*it._Ptr));
			if (script) {
				ImGui::TextColored(ImVec4(0.34f, 0.448f, 0.33f, 1), script->GetName().c_str());
				if (ImGui::BeginPopupContextItem("RemoveScript")) {
					std::stringstream label;
					label << "Remove " << script->GetName() << "##" << script->GetName();
					if (ImGui::Selectable(label.str().c_str())) {
						scriptToRemove = script;
						ImGui::EndPopup();
					}
					else
						ImGui::EndPopup();
				}
				script->OnWidget();
			}
		}
		if (scriptToRemove) {
			node->RemoveComponent(scriptToRemove);
		}

		ImGui::NewLine();
		ImGui::Separator();
	}

	void SceneGraphWidget::PerformCopy()
	{
		if (s_CopySource) {
			if (&m_sceneGraph.GetRoot() != s_CopySource)
				s_selectedNode = &m_sceneGraph.Copy(*s_CopySource);

			s_CopySource = nullptr;
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
		m_ObjectToDelete = nullptr;
	}

	bool SceneGraphWidget::ImguiTreeTraverser::Traverse(GameObject * root)
	{
		m_root = root;

		// root is always open
		ImGui::SetNextTreeNodeOpen(true);

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 2); // Increase spacing to differentiate leaves from expanded contents.
		bool result = root->Accept(*this);
		ImGui::PopStyleVar();

		PerformDrop();
		if (m_ObjectToDelete && m_ObjectToDelete != m_root)
			AIngine::World::DestroyObject(*m_ObjectToDelete);

		return result;
	}

	// Enter a node with children
	bool SceneGraphWidget::ImguiTreeTraverser::Enter(GameObject & node)
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		// selected
		if (s_selectedNode == &node)
		{
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}


		// create a inner node with children
		bool node_open = ImGui::TreeNodeEx(&node, node_flags, node.GetName().c_str());

		CreateContextMenu(node);

		// check if selected
		if (ImGui::IsItemClicked()) {
			s_selectedNode = &node;
			AIngine::Editor::Editor::SetSelectedObjects({ s_selectedNode });
		}

		BeginDragSource();
		BeginDropTarget(node);
		m_openNodesMap[&node] = node_open;

		return node_open;
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

		// create leaf
		ImGui::TreeNodeEx(&node, node_flags, node.GetName().c_str());

		CreateContextMenu(node);

		if (ImGui::IsItemClicked()) {
			s_selectedNode = &node;
			AIngine::Editor::Editor::SetSelectedObjects({ s_selectedNode });
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

	void SceneGraphWidget::ImguiTreeTraverser::CreateContextMenu(GameObject & node)
	{
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::Selectable("Add Child")) {
				AIngine::World::SpawnObject("NewGameObject", &node);
			}
			if (ImGui::Selectable("Duplicate")) {
				s_CopySource = &node;
			}
			if (ImGui::Selectable("Delete")) {
				m_ObjectToDelete = &node;
			}
			ImGui::EndPopup();
		}
	}
}

