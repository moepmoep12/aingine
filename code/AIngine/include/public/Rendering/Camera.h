#include <glm/glm.hpp>
#ifndef M_PI
#define M_PI	3.14159265358979323846f
#endif
namespace AIngine::Rendering {

	class Viewport;

	class Camera {
	public:
		inline const glm::vec2& GetPosition() const { return m_position; }
		inline void SetPosition(const glm::vec2 position) { m_position = position; }
		inline void Translate(const glm::vec2 translation) { m_position += translation; }

		inline float GetRotation() const { return m_rotation; }
		inline void SetRotation(float rot) { m_rotation = rot; }

		inline const float& GetZoom() const { return m_zoom; }
		inline void SetZoom(float value) { m_zoom = value; }

		inline const float& GetZoomMin() const { return m_zoomMin; }
		inline const float& GetZoomMax() const { return m_zoomMax; }
		inline void SetZoomMin(float value) { m_zoomMin = value; }
		inline void SetZoomMax(float value) { m_zoomMax = value; }


		void Zoom(float amount);
		void Rotate(float amount);
		void LookAt(const glm::vec2 pos);
		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetViewMatrix(const glm::vec2& parallaxFactor) const;

		glm::vec2 ScreenToWorldPoint(const glm::vec2& screenpoint) const;
		glm::vec2 WorldToScreenPoint(const glm::vec2& worldpoint) const;

		Camera(const Viewport& viewport);

	private:
		glm::mat4 GetVirtualViewMatrix(const glm::vec2& parallaxFactor) const;

	private:
		glm::vec2 m_position = glm::vec2(0);
		glm::vec2 m_origin = glm::vec2(0);
		float m_rotation = 0.0;
		float m_zoom = 1.0;
		float m_zoomMin = 0.001;
		float m_zoomMax = 1000;
		const Viewport& m_viewport;
	};
}