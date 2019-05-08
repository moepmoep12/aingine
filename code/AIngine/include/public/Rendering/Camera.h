#include <glm/glm.hpp>
#ifndef M_PI
#define M_PI	3.14159265358979323846f
#endif
namespace AIngine::Rendering {

	class Camera {
	public:
		inline const glm::vec2& GetPosition() const { return m_position; }
		inline void SetPosition(const glm::vec2 position) { m_position = position; }
		inline void Translate(const glm::vec2 translation) { m_position += translation; }

		inline float GetRotation() const { return m_rotation; }
		inline void SetRotation(float rot) { m_rotation = rot; }

		void Rotate(float amount);
		const glm::mat4& GetTransform() const;

	private:
		glm::vec2 m_position = glm::vec2(0);
		float m_rotation = 0.0;

	};
}