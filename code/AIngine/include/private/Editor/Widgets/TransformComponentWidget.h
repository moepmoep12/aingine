#pragma once

#include "Editor/Widgets/ComponentWidget.h"
#include "Structures/Traverser.h"


namespace AIngine::Editor {
	class TransformComponentWidget : public ComponentWidget {
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;


		class PhysicsUpdateTraverser : public  AIngine::Traverser {
		public:
			// Inherited via Traverser
			virtual bool Traverse(GameObject * root) override;
			virtual bool Enter(GameObject & node) override;
			virtual bool Leave(GameObject & node) override;
			virtual bool Visit(GameObject & node) override;
		};

	};
}