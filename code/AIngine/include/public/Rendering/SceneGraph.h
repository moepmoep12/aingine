#pragma once
#include "Renderer.h"
#include "memory.h"
#include "Layer.h"
#include <vector>

namespace AIngine::Rendering {
	class SceneGraph : public AIngine::Layer {
	public:

		SceneGraph();
		~SceneGraph();

		GroupNode& GetRoot() { return *m_Root; }

		ShapeNode* AddShape(Texture2D* texture, GroupNode* parent = nullptr, const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f), const float rotation = 0.0f);
		GroupNode* AddGroup(GroupNode* parent = nullptr, const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f), const float rotation = 0.0f);
		bool RemoveNode(SceneNode* node);

		virtual void OnImGuiRender() override;


	private:
		GroupNode* m_Root;
		//std::vector<SceneNode*> m_nodes;
		AIngine::Memory::Pool<ShapeNode> m_shapeNodePool = AIngine::Memory::Pool<ShapeNode>(1000);
		AIngine::Memory::Pool<GroupNode> m_GroupNodePool = AIngine::Memory::Pool<GroupNode>(500);


	};

	class ImguiTreeTraverser : public Traverser {
	public:
		ImguiTreeTraverser(SceneNode* root);

		// Inherited via Traverser
		virtual bool Enter(GroupNode & node) override;
		virtual bool Leave(GroupNode & node) override;
		virtual bool Visit(ShapeNode & node) override;

	};

	class DeleteTraverser : public Traverser {

	public:
		DeleteTraverser(SceneNode* node, AIngine::Memory::Pool<ShapeNode>& shapeNodePool, AIngine::Memory::Pool<GroupNode>& GroupNodePool);
		~DeleteTraverser();

		// Inherited via Traverser
		virtual bool Enter(GroupNode & node) override;
		virtual bool Leave(GroupNode & node) override;
		virtual bool Visit(ShapeNode & node) override;

	private:
		std::vector<ShapeNode*> m_shapeNodesToDelete;
		std::vector<GroupNode*> m_groupNodesToDelete;
		AIngine::Memory::Pool<ShapeNode>* m_shapeNodePool;
		AIngine::Memory::Pool<GroupNode>* m_GroupNodePool;
	};

}