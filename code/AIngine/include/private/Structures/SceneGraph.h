#pragma once
#include "Rendering/Renderer.h"
#include "Structures/memory.h"
#include "Structures/Layer.h"

#include <vector>

namespace AIngine::Editor {
	class Editor;
	namespace Widget {
		class SceneGraphWidget;
	}
}

namespace AIngine::Structures {
	class SceneGraph final : public Layer {

		friend class AIngine::Editor::Widget::SceneGraphWidget;
		friend class AIngine::Editor::Editor;

	public:

		SceneGraph();
		~SceneGraph();

		GameObject& GetRoot() { return *m_Root; }

		virtual void OnUpdate(float delta) override;
		virtual void OnEvent(AIngine::Events::EventData& e) override;
		virtual void OnImGuiRender() override;

		GameObject* const SpawnObject(const std::string& name = std::string("GameObject"), GameObject* parent = nullptr, const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f), const float rotation = 0.0f);

		GameObject& Copy(GameObject& other);

		// Destroys the gameObject and its children by removing it from the scenegraph and freeing its memory
		void DestroyObject(GameObject& gameobject);

		/* Retrieves the GameObject via name */
		GameObject* const GetGameObject(const std::string& name);

	private:
		GameObject* m_Root;
		AIngine::Memory::Pool<GameObject> m_gameObjectPool = AIngine::Memory::Pool<GameObject>(5000);

		void Reset();

		GameObject& CopySingle(GameObject& other, bool keepParent = true);

		// Traverses the SceneGrap from the given root node to create a copy
		class CopyTraverser : public Traverser {
		public:
			CopyTraverser(SceneGraph& graph);
			virtual bool Traverse(GameObject* root) override;
			virtual bool Enter(GameObject & node) override;
			virtual bool Leave(GameObject & node) override;
			virtual bool Visit(GameObject & node) override;

			GameObject* Result = nullptr;

		private:
			SceneGraph& m_sceneGraph;
			std::vector<GameObject*> m_parentStack;
		};

	};

	// Deletes the specified node and the subtree rooted at that node 
	class DeleteTraverser : public Traverser {

	public:
		DeleteTraverser(AIngine::Memory::Pool<GameObject>& gameObjectPool);
		virtual ~DeleteTraverser();

		// Inherited via Traverser

		virtual bool Traverse(GameObject* root) override;

		virtual bool Enter(GameObject & node) override;
		virtual bool Leave(GameObject & node) override;
		virtual bool Visit(GameObject & node) override;

	private:
		std::vector<GameObject*> m_gameObjectsToDelete;
		AIngine::Memory::Pool<GameObject>* m_gameObjectPool;
	};

	// Traverses the SceneGraph to look for a GameObject with specified name
	class SearchForNameTraverser : public Traverser {
	public:
		SearchForNameTraverser(const std::string& name);
		virtual bool Traverse(GameObject* root) override;
		virtual bool Enter(GameObject & node) override;
		virtual bool Leave(GameObject & node) override { return true; }
		virtual bool Visit(GameObject & node) override;

		GameObject* Result = nullptr;

	private:
		std::string m_name;
	};


}

namespace AIngine::UI {

	// Creates a GUI for the scenegraph
	class ImguiTreeTraverser : public Traverser {
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
}