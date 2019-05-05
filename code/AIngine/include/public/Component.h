#include <string>

namespace AIngine {

	class GameObject;

	class Component {
	public:
		inline const std::string& GetName() { return m_name; }
		inline void SetName(const std::string& name) { m_name = name; }

		inline bool IsActive() { return m_isActive; }
		inline void SetActive(bool active) { m_isActive = active; }

		virtual void OnUpdate(float deltatime) {}
		virtual void OnImguiRender() {}

		inline Component(GameObject* owner) : m_owner(owner) {}
		inline virtual ~Component() {}

	protected:
		GameObject* m_owner;

	private:
		std::string m_name;
		bool m_isActive = true;
	};
}