#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "texture.h"

namespace AIngine::Rendering {

	class Traverser;

	class SceneNode {
	public:

		SceneNode() = delete;
		SceneNode(SceneNode* parent, const std::string& name) 
			: m_parent(parent), m_name(name) {}
		SceneNode(SceneNode* parent, const std::string& name, const glm::vec2& position) 
			: m_parent(parent), m_name(name) { SetPosition(position); }
		SceneNode(SceneNode* parent, const std::string& name, const glm::vec2& position, const glm::vec2& scale) 
			: m_parent(parent), m_name(name) { SetPosition(position); SetScale(scale); }
		SceneNode(SceneNode* parent, const std::string& name, const glm::vec2& position, const glm::vec2& scale, const float rotation) 
			: m_parent(parent), m_name(name) { SetPosition(position); SetScale(scale); SetRotation(rotation); }

		virtual bool Accept(Traverser& traverser) = 0;

		inline virtual const std::string& GetName() const { return m_name; }

		inline SceneNode& GetParent() { return *m_parent; }
		inline void SetParent(SceneNode* parent) { m_parent = parent; }

		inline glm::mat4& GetTransform() { return m_transform; }
		inline void SetTransform(const glm::mat4& transform) { m_transform = transform; }

		inline glm::vec2& GetPosition() { return m_position; }
		inline void SetPosition(const glm::vec2 position) { m_position = position; /*glm::translate(m_transform, glm::vec3(m_position, 0.0f));*/ }

		inline glm::vec2& GetScale() { return m_scale; }
		inline void SetScale(const glm::vec2 scale) { m_scale = scale;/* glm::scale(m_transform, glm::vec3(m_scale, 1.0f)); */ }

		inline float GetRotation() { return m_rotation; }
		inline void SetRotation(float rot) { m_rotation = rot;/*  glm::rotate(m_transform, m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));*/ }

	protected:
		std::string m_name;
		glm::mat4 m_transform = glm::mat4(1.0f);
		glm::vec2 m_position = glm::vec2(0.0f);
		glm::vec2 m_scale = glm::vec2(1.0f);
		float m_rotation = 0.0f;
		SceneNode* m_parent;

	private:

	};

	class GroupNode : public SceneNode {
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
			const glm::vec2& position = glm::vec2(0.0f), const glm::vec2& scale = glm::vec2(1.0f), const float rotation = 0.0f);
		~ShapeNode();

		virtual bool Accept(Traverser& traverser) override;

		inline Texture2D& GetTexture() { return *m_texture; }
		glm::vec2& GetSize();


	private:
		Texture2D* m_texture;
		glm::vec2 m_size;
	};
}