#include "AIngine/GameObject.h"
#include "AIngine/Component.h"
#include "Structures/Traverser.h"
#include "Debug/log.h"

namespace AIngine {
	glm::vec2 GameObject::GetWorldPosition() const
	{
		GameObject* parent = m_parent;
		glm::vec2 result = m_position;
		while (parent) {
			result += parent->m_position;
			parent = parent->m_parent;
		}
		return result;
	}
	glm::mat4  GameObject::GetLocalTransform() const
	{
		glm::mat4 result = glm::mat4(1.0);
		result = glm::translate(result, glm::vec3(m_position, 0.0));
		result = glm::rotate(result, m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		result = glm::scale(result, glm::vec3(m_scale, 1.0));
		return result;
	}

	void GameObject::AddChild(GameObject * obj)
	{
		if (obj)
		{
			m_children.push_back(obj);
			obj->SetParent(*this);
		}
	}
	void GameObject::RemoveChild(GameObject * obj)
	{
		auto it = m_children.begin();
		while (it != m_children.end())
		{
			if (*it._Ptr == obj)
			{
				it = m_children.erase(it);

				// to-do: set root as parent
				//obj->SetParent(NULL);

			}
			else
				it++;
		}
	}
	bool GameObject::Accept(Traverser & traverser)
	{
		if (m_children.size() == 0)
		{
			return traverser.Visit(*this);
		}
		else
		{
			if (traverser.Enter(*this))
			{
				for (auto i = m_children.begin(); i != m_children.end(); ++i)
				{
					GameObject* node = *i._Ptr;
					if (!node->Accept(traverser)) {
						break;
					}
				}
			}
			return traverser.Leave(*this);
		}
	}

	GameObject::GameObject(GameObject * parent, const std::string & name)
		: m_parent(parent), m_name(name)
	{
	}

	GameObject::~GameObject()
	{
		for (unsigned int i = 0; i < m_components.size(); ++i) {
			delete m_components[i];
		}
		m_components.clear();
		CORE_INFO("Destroyed {0}", GetName());
	}
}