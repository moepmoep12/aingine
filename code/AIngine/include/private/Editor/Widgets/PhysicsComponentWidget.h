#pragma once

#include "Editor/Widgets/ComponentWidget.h"

namespace AIngine::Physics {
	class PhysicsComponent;
}

namespace AIngine::Editor {
	class PhysicsComponentWidget : public ComponentWidget {
	public:
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;

	private:
		void UpdateBoxVertices(AIngine::Physics::PhysicsComponent& physComp) const;
		void DrawVertex(const glm::vec2& worldPosition);
	};
}