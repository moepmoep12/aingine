#pragma once

#include "Editor/Widgets/EditorWidget.h"
#include "Structures/Traverser.h"

#include <unordered_map>

namespace AIngine {
	class GameObject;
}

namespace AIngine::Structures {
	class SceneGraph;
}

namespace AIngine::Editor {
	class SceneGraphWidget : public EditorWidget {
	public:
		// Inherited via EditorWidget
		virtual void OnImGuiRender() override;
		virtual void OnEvent(AIngine::Events::Event& e) override;

		SceneGraphWidget(AIngine::Structures::SceneGraph& sceneGraph);

	private:
		AIngine::Structures::SceneGraph& m_sceneGraph;

		void ShowSelectedNodeWidget(GameObject* node);



		// Creates a GUI for the scenegraph
		class ImguiTreeTraverser : public AIngine::Traverser {
		public:
			ImguiTreeTraverser();
			virtual ~ImguiTreeTraverser();

			// Inherited via Traverser
			virtual bool Traverse(GameObject* root) override;

			virtual bool Enter(GameObject & node) override;
			virtual bool Leave(GameObject & node) override;
			virtual bool Visit(GameObject & node) override;

			void PerformDrop();

		private:
			// we keep track which nodes are open in the editor 
			// since we need to know when a subtree is finished in imgui
			std::unordered_map<GameObject*, bool> m_openNodesMap;

			void BeginDragSource();
			void BeginDropTarget(GameObject& obj);
		};
	};
}