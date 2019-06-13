#pragma once

#include "Editor/Widgets/EditorWidget.h"
#include "Editor/Widgets/SpriteComponentWidget.h"
#include "Editor/Widgets/TransformComponentWidget.h"
#include "Editor/Widgets/PhysicsComponentWidget.h"
#include "Editor/Widgets/AddComponentWidget.h"
#include "Editor/Widgets/SoundComponentWidget.h"

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
		virtual void OnEvent(AIngine::Events::EventData& e) override;

		virtual ~SceneGraphWidget() override;

		SceneGraphWidget(AIngine::Structures::SceneGraph& sceneGraph);

	private:
		AIngine::Structures::SceneGraph& m_sceneGraph;
		SpriteComponentWidget* m_textureCompWidget;
		TransformComponentWidget* m_transformCompWidget;
		PhysicsComponentWidget* m_physCompWidget;
		AddComponentWidget* m_addComponentWidget;
		SoundComponentWidget* m_soundComponentWidget;

		void ShowSelectedNodeWidget(GameObject* node);
		void ShowUserScripts(GameObject* node);

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

			GameObject* m_root;

			GameObject* m_ObjectToDelete = nullptr;
			GameObject* m_ObjectToMoveUp = nullptr;
			GameObject* m_ObjectToMoveDown = nullptr;

			void BeginDragSource();
			void BeginDropTarget(GameObject& obj);
			void PerformGameObjectReposition();
		};

		
	};
}