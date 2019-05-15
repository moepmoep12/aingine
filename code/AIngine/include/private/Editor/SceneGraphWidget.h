#include "Editor/ImGuiWidget.h"
#include "Structures/Traverser.h"
#include <unordered_map>

namespace AIngine {
	class SceneGraph;
	class GameObject;
}

namespace AIngine::Editor {
	class SceneGraphWidget : public ImGuiWidget {
	public:
		// Inherited via ImGuiWidget
		virtual void OnImGuiRender() override;
		virtual void OnEvent(AIngine::Events::Event& e) override;

		SceneGraphWidget(SceneGraph& sceneGraph);

	private:
		SceneGraph& m_sceneGraph;

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

		private:
			// we keep track which nodes are open in the editor 
			// since we need to know when a subtree is finished in imgui
			std::unordered_map<GameObject*, bool> m_openNodesMap;


		};
	};
}