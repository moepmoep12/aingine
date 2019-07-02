#pragma once

#include "Editor/Widgets/ComponentWidget.h"
#include "Structures/Traverser.h"


namespace AIngine::Editor::Widget::Component {
	class TransformComponentWidget : public ComponentWidget {
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;

	private:
		GameObject* m_ObjectToMoveUp = nullptr;
		GameObject* m_ObjectToMoveDown = nullptr;

		void PerformGameObjectReposition(GameObject* obj);

		class PhysicsUpdateTraverser : public  AIngine::Traverser {
		public:
			// Inherited via Traverser
			virtual bool Traverse(GameObject * root) override;
			virtual bool Enter(GameObject & node) override;
			virtual bool Leave(GameObject & node) override;
			virtual bool Visit(GameObject & node) override;

		private:
		};

	};
}