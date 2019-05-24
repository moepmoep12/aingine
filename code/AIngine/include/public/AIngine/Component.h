#pragma once
#include <string>
#include <glm/glm.hpp>

namespace AIngine {

	class GameObject;

	class Component {
	public:
		inline const std::string& GetName() { return m_name; }
		inline void SetName(const std::string& name) { m_name = name; }

		inline bool IsActive() const { return m_isActive; }
		virtual void SetActive(bool active) { m_isActive = active; }

		virtual void OnUpdate(float deltatime) {}
		virtual void OnImguiRender() {}

		virtual void OnOwnerTransformChanged(const glm::vec2 newPosition, const glm::vec2& newScale, const float& newRotation) {}
		virtual void OnOwnerLocalPositionChanged(const glm::vec2& position) {}
		virtual void OnOwnerLocalScaleChanged(const glm::vec2& scale) {}
		virtual void OnOwnerLocalRotationChanged(const float& rot) {}
		virtual void OnOwnerParentChanged(const GameObject& newParent) {}

		inline GameObject* GetOwner() { return m_owner; }

		inline Component(GameObject* owner) : m_owner(owner) {}
		inline virtual ~Component() {}

	protected:
		GameObject* m_owner;

	private:
		std::string m_name;
		bool m_isActive = true;
	};
}