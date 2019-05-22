#pragma once
//#include "Component.h"
#include <vector>
#include <string>
#include <typeinfo>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace AIngine {

	// forward declaration
	class Traverser;
	class SceneGraph;
	class Component;

	class GameObject {
	public:
		inline const std::vector<Component*>& GetComponents() { return m_components; }

		template <class T>
		inline T* GetComponent()
		{
			auto it = m_components.begin();

			while (it != m_components.end())
			{
				if (typeid(T) == typeid(**it._Ptr)) {
					return dynamic_cast<T*>(*it._Ptr);
				}
				it++;
			}

			return nullptr;
		}

		template<class T>
		inline void RemoveComponent() {
			auto it = m_components.begin();

			while (it != m_components.end())
			{
				if (typeid(T) == typeid(*it._Ptr)) {
					it = m_components.erase(it);
					delete (*it._Ptr);
					return;
				}
				it++;
			}
		}

		template <class T>
		inline T* AddComponent()
		{
			T* comp = new T(this);

			m_components.push_back(comp);

			return comp;
		}

		inline const std::string& GetName() { return m_name; }
		inline void SetName(const std::string& name) { m_name = name; }

		inline GameObject& GetParent() { return *m_parent; }
		void SetParent(GameObject&  parent);

		inline glm::vec2& GetLocalPosition() { return m_position; }
		void SetLocalPosition(const glm::vec2& position);
		void Translate(const glm::vec2& translation);

		inline glm::vec2& GetLocalScale() { return m_scale; }
		void SetLocalScale(const glm::vec2& scale);

		inline float& GetLocalRotation() { return m_rotation; }
		void SetRotation(float rot);
		void Rotate(float amount);

		inline const std::vector<GameObject*>& GetChildren() { return m_children; }

		glm::mat4 GetLocalTransform() const;
		glm::vec2 GetWorldPosition() const;
		glm::vec2 GetWorldScale() const;
		float GetWorldRotation() const;
		void AddChild(GameObject* obj);
		void RemoveChild(GameObject* obj);

		virtual void OnUpdate(float deltatime) {}
		virtual bool Accept(Traverser& traverser);

		GameObject() = delete;
		GameObject(GameObject* parent, const std::string& name);
		~GameObject();

	private:
		std::vector<Component*> m_components;
		std::vector<GameObject*> m_children;
		std::string m_name;
		glm::vec2 m_position = glm::vec2(0.0f);
		glm::vec2 m_scale = glm::vec2(1.0f);
		float m_rotation = 0.0f;
		GameObject* m_parent;


	protected:
		friend class SceneGraph;

		//~GameObject();
	};
}