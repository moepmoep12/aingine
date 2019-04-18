#include "Rendering/SceneNode.h"
#include "Rendering/Traverser.h"
#include "log.h"

namespace AIngine::Rendering {

	ShapeNode::ShapeNode(Texture2D* texture, GroupNode* parent, const std::string& name, const glm::vec2& position, const glm::vec2& scale, const float rotation)
		: m_texture(texture), SceneNode(parent,name)
	{
		SetPosition(position);
		SetScale(scale);
		SetRotation(rotation);	
	}

	ShapeNode::~ShapeNode()
	{
		CORE_INFO("Destroying {0}", GetName());
	}

	bool ShapeNode::Accept(Traverser& traverser) {
		return traverser.Visit(*this);
	}

	glm::vec2 & ShapeNode::GetSize()
	{
		m_size = glm::vec2(m_texture->Width, m_texture->Height);
		return m_size;
	}

	/************************************ GROUP NODE ******************************/

	GroupNode::~GroupNode()
	{
		//for (unsigned int i = 0; i < m_children.size(); ++i) {
		//	delete m_children[i];
		//}
		CORE_INFO("Destroying {0}", GetName());
		m_children.clear();
	}

	bool GroupNode::Accept(Traverser& traverser) {
		if (traverser.Enter(*this)) {
			for (auto i = m_children.begin(); i != m_children.end(); ++i)
			{
				SceneNode* node = *i._Ptr;
				if (!node->Accept(traverser)) {
					break;
				}
			}
		}

		return traverser.Leave(*this);
	}
	void GroupNode::AddChild(SceneNode * node)
	{
		if (node) {
			m_children.push_back(node);
			node->SetParent(this);
		}
	}
	void GroupNode::RemoveChild(SceneNode * node)
	{
		auto it = m_children.begin();
		while (it != m_children.end())
		{
			if (*it == node)
			{
				it = m_children.erase(it);
				node->SetParent(nullptr);
			}
			else
				it++;
		}
	}
	unsigned int GroupNode::ChildrenInSubTree()
	{
		int count = 0;

		for (auto i = m_children.begin(); i != m_children.end(); ++i)
		{
			GroupNode* node = dynamic_cast<GroupNode*>(*i._Ptr);
			if (node) {
				count += node->ChildrenInSubTree();
			}

		}

		count += m_children.size();

		return count;
	}
}