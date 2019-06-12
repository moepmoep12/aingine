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

			float* position[] = { &activeGameObject->GetLocalPosition().x ,&activeGameObject->GetLocalPosition().y };
			float* scale[] = { &activeGameObject->GetLocalScale().x ,&activeGameObject->GetLocalScale().y };
			float* rot = &activeGameObject->GetLocalRotation();
			float rotDegree = activeGameObject->GetLocalRotation() * R2D;

			const float translationSliderSpeed = 0.1f;
			const float scaleSliderSpeed = 0.1f;
			const float rotationSliderSpeed = 0.5f;

			// rename
			static char str0[40] = "Enter Name";
			if (ImGui::InputText("Name", str0, IM_ARRAYSIZE(str0))) {
				activeGameObject->SetName(str0);
			}

			ImGui::Separator();

			ImGui::BulletText("Transform");
			ImGui::DragFloat2("Position", *position, translationSliderSpeed);
			ImGui::DragFloat2("Scale", *scale, scaleSliderSpeed);
			ImGui::DragFloat("Rotation", &rotDegree, rotationSliderSpeed);

			// create vertex at the objects center to move it around with the mouse
			if (activeGameObject != &AIngine::Editor::Editor::GetSceneGraph()->GetRoot())
				AIngine::Editor::Editor::CreateMoveablePositionVertex(activeGameObject->GetLocalPosition(), 15, glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));

			glm::vec2 newPosition = activeGameObject->GetLocalPosition();
			glm::vec newScale = activeGameObject->GetLocalScale();


			// the transform of the object was changed, we need to propagate this
			if (newPosition != originalPosition || *rot != rotDegree * D2R) {
				*rot = std::fmodf(rotDegree * D2R, 2 * M_PI);
				PhysicsUpdateTraverser physUpdate;
				physUpdate.Traverse(activeGameObject);
			}
		}
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