#include "AIngine/GameObject.h"
#include "AIngine/Component.h"
#include "Structures/Traverser.h"
#include "AIngine/Constants.h"

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

	void GameObject::SetWorldPosition(const glm::vec2& pos, bool bInformComponents)
	{
		glm::vec2 worldPos = GetWorldPosition();
		glm::vec2 diff = pos - worldPos;
		Translate(diff, bInformComponents);
	}

	glm::vec2 GameObject::GetWorldScale() const
	{
		GameObject* parent = m_parent;
		glm::vec2 result = m_scale;
		while (parent) {
			result *= parent->m_scale;
			parent = parent->m_parent;
		}
		return result;
	}
	float GameObject::GetWorldRotation() const
	{
		GameObject* parent = m_parent;
		float result = m_rotation;
		while (parent) {
			result += parent->m_rotation;
			parent = parent->m_parent;
		}
		return result;
	}

	void GameObject::SetWorldRotation(float rot, bool bInformComponents)
	{
		float worldRot = GetWorldRotation();
		float diff = rot - worldRot;
		Rotate(diff, bInformComponents);
	}

	void GameObject::SetParent(GameObject & parent, bool bInformComponents)
	{
		m_parent = &parent;
		if (bInformComponents) {
			auto it = m_components.begin();
			while (it != m_components.end()) {
				(*it._Ptr)->OnOwnerParentChanged(parent);
				it++;
			}
		}
	}

	void GameObject::SetLocalPosition(const glm::vec2 & position, bool bInformComponents)
	{
		m_position = position;
		if (bInformComponents) {
			auto it = m_components.begin();
			while (it != m_components.end()) {
				(*it._Ptr)->OnOwnerLocalPositionChanged(position);
				it++;
			}
		}
	}

	void GameObject::Translate(const glm::vec2 & translation, bool bInformComponents)
	{
		m_position += translation;
		if (bInformComponents) {
			auto it = m_components.begin();
			while (it != m_components.end()) {
				(*it._Ptr)->OnOwnerLocalPositionChanged(m_position);
				it++;
			}
		}
	}

	void GameObject::SetLocalScale(const glm::vec2 & scale, bool bInformComponents)
	{
		m_scale = scale;
		if (bInformComponents) {
			auto it = m_components.begin();
			while (it != m_components.end()) {
				(*it._Ptr)->OnOwnerLocalScaleChanged(scale);
				it++;
			}
		}
	}

	void GameObject::Scale(const glm::vec2 & amount, bool bInformComponents)
	{
		m_scale += amount;
		if (bInformComponents) {
			auto it = m_components.begin();
			while (it != m_components.end()) {
				(*it._Ptr)->OnOwnerLocalScaleChanged(m_scale);
				it++;
			}
		}
	}

	void GameObject::SetRotation(float rot, bool bInformComponents)
	{
		m_rotation = std::fmodf(m_rotation + rot, 2* M_PI);
		if (bInformComponents) {
			auto it = m_components.begin();
			while (it != m_components.end()) {
				(*it._Ptr)->OnOwnerLocalRotationChanged(rot);
				it++;
			}
		}
	}

	void GameObject::Rotate(float amount, bool bInformComponents)
	{
		m_rotation = std::fmodf(m_rotation + amount, 2* M_PI);
		if (bInformComponents) {
			auto it = m_components.begin();
			while (it != m_components.end()) {
				(*it._Ptr)->OnOwnerLocalRotationChanged(m_rotation);
				it++;
			}
		}
	}

	void GameObject::UpdateTransform(const glm::vec2 & translation, const glm::vec2 & scale, float rot)
	{
		m_position += translation;
		m_scale += scale;
		m_rotation += rot;
		auto it = m_components.begin();
		while (it != m_components.end()) {
			(*it._Ptr)->OnOwnerTransformChanged(m_position, m_scale, m_rotation);
			it++;
		}
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
	void GameObject::AddChild(std::vector<GameObject*>::const_iterator _where, GameObject * obj)
	{
		if (obj) {
			m_children.insert(_where, obj);
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
				break;
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
					node->Accept(traverser);
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
	}
}