#pragma once

#include "Editor/Widgets/ComponentWidget.h"

#include "Box2D/Box2D.h"

namespace AIngine::Physics {
	class PhysicsComponent;
}

namespace AIngine::Editor::Widget::Component {
	class PhysicsComponentWidget : public ComponentWidget {
	public:
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;

	private:
		void UpdateBoxVertices(AIngine::Physics::PhysicsComponent& physComp) const;
		void CreateMoveableVertex(glm::vec2& localPosition, const b2Transform& transform, float size, const glm::vec3& colorInteract = glm::vec3(0,1,0), const glm::vec3& colorNormal = glm::vec3(1,0,0));
		//void DrawVertex(const glm::vec2& worldPosition, const glm::vec3& color = glm::vec3(1, 0, 0));

		void CreateCircleUI(AIngine::Physics::PhysicsComponent* physComp);
		void CreateBoxUI(AIngine::Physics::PhysicsComponent* physComp);
		void CreatePolygonUI(AIngine::Physics::PhysicsComponent* physComp);
		void CreateEdgeUI(AIngine::Physics::PhysicsComponent* physComp);

		glm::vec2 CalculateWorldPosition(const glm::vec2& localPos, const b2Transform& transform);
	};
}