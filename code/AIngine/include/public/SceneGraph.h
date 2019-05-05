#pragma once
#include "Rendering/Renderer.h"
#include "memory.h"
#include "Layer.h"
#include <vector>

namespace AIngine {
	class SceneGraph : public AIngine::Layer {
	public:

		SceneGraph();
		~SceneGraph();

		GameObject& GetRoot() { return *m_Root; }

		virtual void OnImGuiRender() override;

		GameObject* const SpawnObject(const std::string& name = std::string("GameObject"), GameObject* parent = nullptr, const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f), const float rotation = 0.0f);
		//GameObject* const FindObject(const std::string& name);

		// Destroys the gameObject and its children by removing it from the scenegraph and freeing its memory
		void DestroyObject(GameObject& gameobject);

	private:
		GameObject* m_Root;
		AIngine::Memory::Pool<GameObject> m_gameObjectPool = AIngine::Memory::Pool<GameObject>(1000);

	private:
		void ShowSelectedNodeWidget(GameObject* node);

	};

	// traverses 
	class DeleteTraverser : public Traverser {

	public:
		DeleteTraverser(GameObject* node, AIngine::Memory::Pool<GameObject>& gameObjectPool);
		~DeleteTraverser();

		// Inherited via Traverser
		virtual bool Enter(GameObject & node) override;
		virtual bool Leave(GameObject & node) override;
		virtual bool Visit(GameObject & node) override;

	private:
		std::vector<GameObject*> m_gameObjectsToDelete;
		AIngine::Memory::Pool<GameObject>* m_gameObjectPool;
	};


}

namespace AIngine::UI {

	// Creates a GUI for the scenegraph
	class ImguiTreeTraverser : public Traverser {
	public:
		ImguiTreeTraverser(GameObject* root);
		~ImguiTreeTraverser();

		// Inherited via Traverser
		virtual bool Enter(GameObject & node) override;
		virtual bool Leave(GameObject & node) override;
		virtual bool Visit(GameObject & node) override;

		void Traverse();

	private:
		GameObject* m_root;

		// we keep track which nodes are open in the editor 
		// since we need to know when a subtree is finished in imgui
		std::unordered_map<GameObject*, bool> m_openNodesMap;


	};
}