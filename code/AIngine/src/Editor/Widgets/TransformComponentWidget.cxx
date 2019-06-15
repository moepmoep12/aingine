#include "Editor/Widgets/TransformComponentWidget.h"
#include "AIngine/GameObject.h"
#include "AIngine/Constants.h"
#include "AIngine/Physics.h"
#include "Editor/Editor.h"
#include "Structures/SceneGraph.h"

namespace AIngine::Editor {
	void TransformComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1)
		{
			AIngine::GameObject* activeGameObject = m_activeGameObjects[0];

			glm::vec2 originalPosition = activeGameObject->GetLocalPosition();
			glm::vec2 originalScale = activeGameObject->GetLocalScale();

			float* position[] = { &activeGameObject->m_position.x ,&activeGameObject->m_position.y };
			float* scale[] = { &activeGameObject->m_scale.x ,&activeGameObject->m_scale.y };
			float* rot = &activeGameObject->m_rotation;
			float rotDegree = activeGameObject->GetLocalRotation() * R2D;

			const float translationSliderSpeed = 0.1f;
			const float scaleSliderSpeed = 0.1f;
			const float rotationSliderSpeed = 0.5f;

			ImGui::Separator();

			// active
			ImGui::Checkbox("Active##gameobject", &activeGameObject->m_active);

			// rename
			static char str0[40] = "Enter Name";
			if (ImGui::InputText("Name", str0, IM_ARRAYSIZE(str0))) {
				activeGameObject->SetName(str0);
			}

			GameObject* root = &AIngine::Editor::Editor::GetSceneGraph()->GetRoot();
			if (activeGameObject != root) {
				ImGui::SameLine();
				// create buttons for gameobject order
				if (ImGui::ArrowButton("ButtonUp##gameObject", ImGuiDir_Up)) {
					m_ObjectToMoveUp = activeGameObject;
				}
				ImGui::SameLine();
				if (ImGui::ArrowButton("ButtonDown##gameObject", ImGuiDir_Down)) {
					m_ObjectToMoveDown = activeGameObject;
				}
			}

			ImGui::Separator();

			// Component Title
			float textWidth = ImGui::CalcTextSize("Transform Component").x;
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) * 0.5f);
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform Component");

			ImGui::NewLine();

			// Transform Values
			ImGui::DragFloat2("Position", *position, translationSliderSpeed);
			ImGui::DragFloat2("Scale", *scale, scaleSliderSpeed);
			ImGui::DragFloat("Rotation", &rotDegree, rotationSliderSpeed);

			// create vertex at the objects center to move it around with the mouse
			if (activeGameObject != &AIngine::Editor::Editor::GetSceneGraph()->GetRoot())
				AIngine::Editor::Editor::CreateMoveablePositionVertex(activeGameObject->m_position, 15, glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));

			glm::vec2 newPosition = activeGameObject->GetLocalPosition();
			glm::vec newScale = activeGameObject->GetLocalScale();


			// the transform of the object was changed, we need to propagate this
			if (newPosition != originalPosition || *rot != rotDegree * D2R) {
				*rot = std::fmodf(rotDegree * D2R, 2 * M_PI);
				PhysicsUpdateTraverser physUpdate;
				physUpdate.Traverse(activeGameObject);
			}

			PerformGameObjectReposition(activeGameObject);

			ImGui::Separator();
		}
	}

	void TransformComponentWidget::PerformGameObjectReposition(GameObject* obj)
	{
		GameObject* root = &AIngine::Editor::Editor::GetSceneGraph()->GetRoot();

		if (m_ObjectToMoveUp && m_ObjectToMoveUp != root) {
			AIngine::GameObject* parent = m_ObjectToMoveUp->GetParent();
			int index = 0;
			int i = 0;

			for (auto child : parent->GetChildren()) {
				if (child == m_ObjectToMoveUp) {
					index = i - 1;
					break;
				}
				i++;
			}

			if (index >= 0) {
				parent->RemoveChild(m_ObjectToMoveUp);
				parent->AddChild(parent->GetChildren().begin() + index, m_ObjectToMoveUp);
			}
		}

		if (m_ObjectToMoveDown && m_ObjectToMoveDown != root) {
			AIngine::GameObject* parent = m_ObjectToMoveDown->GetParent();
			int index = 0;
			int i = 0;

			for (auto child : parent->GetChildren()) {
				if (child == m_ObjectToMoveDown) {
					index = i + 1;
					break;
				}
				i++;
			}

			if (index < parent->GetChildren().size()) {
				parent->RemoveChild(m_ObjectToMoveDown);
				parent->AddChild(parent->GetChildren().begin() + index, m_ObjectToMoveDown);
			}
		}

		m_ObjectToMoveUp = nullptr;
		m_ObjectToMoveDown = nullptr;
	}

	/*------------------------------------------- PHYSICSUPDATE TRAVERSER -----------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------------------*/


	bool TransformComponentWidget::PhysicsUpdateTraverser::Traverse(GameObject * root)
	{
		bool result = root->Accept(*this);
		return result;
	}
	bool TransformComponentWidget::PhysicsUpdateTraverser::Enter(GameObject & node)
	{
		Physics::PhysicsComponent* physComp = node.GetComponent < Physics::PhysicsComponent>();
		if (physComp) {
			physComp->OnOwnerTransformChanged(glm::vec2(0.0), glm::vec2(1.0), 0);
		}

		return true;
	}
	bool TransformComponentWidget::PhysicsUpdateTraverser::Leave(GameObject & node)
	{
		return true;
	}
	bool TransformComponentWidget::PhysicsUpdateTraverser::Visit(GameObject & node)
	{
		Physics::PhysicsComponent* physComp = node.GetComponent < Physics::PhysicsComponent>();
		if (physComp) {
			physComp->OnOwnerTransformChanged(glm::vec2(0.0), glm::vec2(1.0), 0);
		}
		return true;
	}

}