#pragma once
#include "Events/Event.h"
#include "GameObject.h"
#include <string>
#include <glm/glm.hpp>

namespace AIngine {

	namespace Structures {
		class SceneGraph;
		class UpdateTraverser;
	}


	class Component {
	public:
		inline  std::string GetName() const { return m_name; }
		inline void SetName(const std::string& name) { m_name = name; }

		/* Returns whether this component is enabled */
		inline bool IsEnabled() const { return m_isEnabled; }
		virtual void SetEnabled(bool enabled) { m_isEnabled = enabled; }

		/* Returns whether this component is enabled AND all parents of the owner */
		inline bool IsActive() const {
			if (!m_owner->IsActive()) return false;

			GameObject* parent = m_owner->GetParent();
			while (parent) {
				if (!parent->IsActive()) return false;
				parent = parent->GetParent();
			}

			return m_isEnabled;
		}

		inline void Destroy() { m_wantsDestroy = true; }

		virtual void OnEvent(AIngine::Events::EventData& e) {}
		virtual void OnUpdate(float deltatime) {}
		virtual void OnImGuiRender() {}

		template<class T>
		inline T* GetComponent() {
			return m_owner->GetComponent<T>();
		}

		inline const std::vector<Component*>& GetComponents()
		{
			return m_owner->GetComponents();
		}

		inline GameObject* const GetParent() {
			return m_owner->GetParent();
		}

		inline GameObject* GetChild(const std::string& name) {
			return m_owner->GetChild(name);
		}


		inline GameObject* GetOwner() {
			return m_owner;
		}

		Component(const Component& other) = delete;
		Component(Component&& other) = delete;
		Component& operator=(const Component& other) = delete;
		Component& operator=(Component&& other) = delete;


	protected:
		GameObject* m_owner = nullptr;

		friend class AIngine::Structures::SceneGraph;
		friend class AIngine::Structures::UpdateTraverser;
		friend class AIngine::GameObject;

		inline Component() {}
		inline virtual ~Component() {}

		/* To be called by GameObject AFTER constructing a Component*/
		virtual void PostInit() {}

		virtual void OnOwnerTransformChanged(const glm::vec2 newPosition, const glm::vec2& newScale, const float& newRotation) {}
		virtual void OnOwnerLocalPositionChanged(const glm::vec2& position) {}
		virtual void OnOwnerLocalScaleChanged(const glm::vec2& scale) {}
		virtual void OnOwnerLocalRotationChanged(const float& rot) {}
		virtual void OnOwnerParentChanged(const GameObject& newParent) {}

		virtual Component* Copy(GameObject* const owner) const { return nullptr; }


	private:
		std::string m_name;
		bool m_isEnabled = true;
		bool m_wantsDestroy = false;


	};
}