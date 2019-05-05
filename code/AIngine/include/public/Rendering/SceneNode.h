#pragma once

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <vector>
//#include "texture.h"
//#include "GameObject.h"

namespace AIngine::Rendering {

	//class Traverser;

	//class SceneNode {
	//public:

	//	SceneNode() = delete;
	//	SceneNode(SceneNode* parent, GameObject* obj)
	//		: m_parent(parent), m_gameObject(obj) {}


	//	inline SceneNode& GetParent() { return *m_parent; }
	//	inline void SetParent(SceneNode* parent) { m_parent = parent; }

	//	inline const std::vector<SceneNode*>& GetChildren() { return m_children; }

	//	virtual bool Accept(Traverser& traverser);
	//	virtual void AddChild(SceneNode* node);
	//	virtual void RemoveChild(SceneNode* node);

	//protected:
	//	GameObject* m_gameObject;
	//	SceneNode* m_parent;
	//	std::vector<SceneNode*> m_children;
	//private:

	//};

	/*class GroupNode : public SceneNode {
	public:

		using SceneNode::SceneNode;

		~GroupNode();

		virtual bool Accept(Traverser& traverser) override;

		inline const std::vector<SceneNode*>& GetChildren() { return m_children; }

		void AddChild(SceneNode* node);
		void RemoveChild(SceneNode* node);

		unsigned int ChildrenInSubTree();

	private:
		std::vector<SceneNode*> m_children;
	};

	class ShapeNode : public SceneNode {
	public:

		explicit ShapeNode(Texture2D* texture, GroupNode* parent, const std::string& name,
			const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f), const float rotation = 0.0f, const glm::vec3& color = glm::vec3(1.0f));
		~ShapeNode();

		virtual bool Accept(Traverser& traverser) override;

		inline Texture2D& GetTexture() { return *m_texture; }
		glm::vec2& GetSize();

		inline glm::vec3& GetColor() { return m_spriteColor; }
		inline void SetColor(const glm::vec3& color) { m_spriteColor = color; }


	private:
		Texture2D* m_texture;
		glm::vec2 m_size;
		glm::vec3 m_spriteColor;
	};*/
}