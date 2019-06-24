#pragma once

#include "Structures/Traverser.h"

#include <vector>
#include <string>
#include <typeinfo>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace AIngine {

	// forward declarations
	class Component;

	namespace Structures {
		class SceneGraph;
	}

	namespace Editor {
		class TransformComponentWidget;
	}

	namespace Events {
		class EventData;
	}

	/// A GameObject represents an entity wihin a game
   /// It acts as a container for Components
	class GameObject final {
	public:
		inline const std::vector<Component*>& GetComponents() { return m_components; }

		/* Returns the component by type*/
		template <class T>
		inline T* const GetComponent()
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

		/* Removes the component specified by type*/
		template<class T>
		inline void RemoveComponent() {
			auto it = m_components.begin();

			while (it != m_components.end())
			{
				if (typeid(T) == typeid(**it._Ptr)) {
					T* comp = (T*)(*it._Ptr);
					m_components.erase(it);
					delete comp;
					return;
				}
				it++;
			}
		}

		/* Adds a new component of type T*/
		template <class T>
		inline T* const AddComponent()
		{
			if (GetComponent<T>())
				return GetComponent<T>();

			T* comp = new T();
			comp->m_owner = this;

			m_components.push_back(comp);

			return comp;
		}

		/* Removes the specified component by pointer*/
		void RemoveComponent(Component* comp);

		/* The name of this GameObject*/
		inline const std::string& GetName() const { return m_name; }
		inline void SetName(const std::string& name) { m_name = name; }

		inline bool IsActive() const { return m_active; }
		inline void SetActive(bool active) { m_active = active; }

		/* The parent GameObject of this GameObject*/
		inline GameObject* const GetParent() { return m_parent; }
		void SetParent(GameObject&  parent, bool bInformComponents = true);

		/* Returns the local position of this GameObject*/
		inline glm::vec2 GetLocalPosition() const { return m_position; }
		/* Sets the local position of this gameObject
		* @bInformComponents: whether to propagate the change to the GameObjects components */
		void SetLocalPosition(const glm::vec2& position, bool bInformComponents = true);
		/* Translates the gameobject by the specified amount
		* @bInformComponents: whether to propagate the change to the GameObjects components */
		void Translate(const glm::vec2& translation, bool bInformComponents = true);

		/* The local Scale of this GameObject*/
		inline glm::vec2 GetLocalScale() const { return m_scale; }
		/* Sets the local scale of this gameObject
		* @bInformComponents: whether to propagate the change to the GameObjects components */
		void SetLocalScale(const glm::vec2& scale, bool bInformComponents = true);
		/* Scales this GameObject multiplicative by the amount
		* @bInformComponents: whether to propagate the change to the GameObjects components */
		void Scale(const glm::vec2& amount, bool bInformComponents = true);

		/* The local rotation of this GameObject given in radians between -2PI and 2PI */
		inline float GetLocalRotation() const { return m_rotation; }
		/* Sets the local rotation of this gameObject
		* @bInformComponents: whether to propagate the change to the GameObjects components */
		void SetRotation(float rot, bool bInformComponents = true);
		/* Rotates this GameObject additively by amount
		* @bInformComponents: whether to propagate the change to the GameObjects components */
		void Rotate(float amount, bool bInformComponents = true);

		/* Sets the transform of this GameObject by applying translation,scaling & rotation */
		void UpdateTransform(const glm::vec2& translation, const glm::vec2& scale, float rot);

		/* Returns all GameObject this GameObject is parent of */
		inline const std::vector<GameObject*>& GetChildren() { return m_children; }

		/* Returns this GameObjects transform as a 4x4 Matrix*/
		glm::mat4 GetLocalTransform() const;
		/* Returns this GameObjects world position */
		glm::vec2 GetWorldPosition() const;
		/* Sets the world position of this gameObject
		* @bInformComponents: whether to propagate the change to the GameObjects components */
		void SetWorldPosition(const glm::vec2& pos, bool bInformComponents = true);
		/* Returns the world scale of this GameObject */
		glm::vec2 GetWorldScale() const;
		/* Returns the world rotation of this GameObject
		* note: Parent rotations stack additively */
		float GetWorldRotation() const;
		/* Sets the world rotation of this gameObject
		* @bInformComponents: whether to propagate the change to the GameObjects components */
		void SetWorldRotation(float rot, bool bInformComponents = true);

		/* Adds a child to the end of this GameObjects child list */
		void AddChild(GameObject* obj);
		/* Adds a child at a specified position */
		void AddChild(std::vector<GameObject*>::const_iterator _where, GameObject* obj);
		/* Removes a child by pointer*/
		void RemoveChild(GameObject* obj);
		/* Retrieves a child by name */
		GameObject* GetChild(const std::string& name);

		/* Visitor Pattern for traversal */
		virtual bool Accept(Traverser& traverser);

		// delete creation of GameObjects outside of World/SceneGraph
		GameObject() = delete;
		GameObject(const GameObject& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;
		~GameObject();

	private:
		std::vector<Component*> m_components;
		std::vector<GameObject*> m_children;
		std::string m_name;
		glm::vec2 m_position = glm::vec2(0.0f);
		glm::vec2 m_scale = glm::vec2(1.0f);
		float m_rotation = 0.0f;
		bool m_active = true;
		GameObject* m_parent;

		/* SceneGraph is responsible for the creation of GameObjects */
		friend class AIngine::Structures::SceneGraph;

		friend class AIngine::Editor::TransformComponentWidget;

		// private Constructor to be used by the SceneGraph
		GameObject(GameObject* parent, const std::string& name);

		// The UpdateTraverser traverses the scenegraph to update every active gameobject & component
		class UpdateTraverser : private Traverser {
		public:
			UpdateTraverser(float deltaTime);
			virtual ~UpdateTraverser();

			// Inherited via Traverser
			virtual bool Traverse(GameObject* root) override;
			virtual bool Enter(GameObject & node) override;
			virtual bool Leave(GameObject & node) override;
			virtual bool Visit(GameObject & node) override;

		private:
			float m_deltaTime;
		};

		// Traverses the SceneGraph to propagate events
		class EventTraverser : private Traverser {
		public:
			EventTraverser(AIngine::Events::EventData& e);
			virtual ~EventTraverser();

			// Inherited via Traverser

			virtual bool Traverse(GameObject* root) override;
			virtual bool Enter(GameObject & node) override;
			virtual bool Leave(GameObject & node) override;
			virtual bool Visit(GameObject & node) override;

		private:
			AIngine::Events::EventData& m_eventData;
		};

		// Traverses the SceneGraph to call the OnGui method
		class OnGUITraverser : private Traverser {
		public:
			// Inherited via Traverser
			virtual bool Traverse(GameObject* root) override;
			virtual bool Enter(GameObject & node) override;
			virtual bool Leave(GameObject & node) override { return true; }
			virtual bool Visit(GameObject & node) override;
		};
	};
}