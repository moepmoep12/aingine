#include "Editor/Widgets/TransformComponentWidget.h"
#include "AIngine/GameObject.h"
#include "AIngine/Constants.h"
#include "AIngine/Physics.h"

namespace AIngine::Editor {
	void TransformComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1)
		{
			AIngine::GameObject* activeGameObject = m_activeGameObjects[0];

			glm::vec2 originalPosition = activeGameObject->GetLocalPosition();
			glm::vec2 originalScale = activeGameObject->GetLocalScale();
			float originalRot = activeGameObject->GetLocalRotation();

			float* position[] = { &activeGameObject->GetLocalPosition().x ,&activeGameObject->GetLocalPosition().y };
			float* scale[] = { &activeGameObject->GetLocalScale().x ,&activeGameObject->GetLocalScale().y };
			float* rot = &activeGameObject->GetLocalRotation();

			const float translationSliderSpeed = 0.1f;
			const float scaleSliderSpeed = 0.1f;
			const float rotationSliderSpeed = M_PI / 180.0f;

			ImGui::BulletText("Transform");
			ImGui::DragFloat2("Position", *position, translationSliderSpeed);
			ImGui::DragFloat2("Scale", *scale, scaleSliderSpeed);
			ImGui::DragFloat("Rotation", rot, rotationSliderSpeed);


			glm::vec2 newPosition = activeGameObject->GetLocalPosition();
			glm::vec newScale = activeGameObject->GetLocalScale();


			// the transform of the object was changed, we need to propagate this
			if (newPosition != originalPosition || *rot != originalRot) {
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