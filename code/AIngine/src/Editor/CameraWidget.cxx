#include "Editor/CameraWidget.h"
#include "AIngine/Input.h"
#include <glm/glm.hpp>

namespace AIngine::Editor {
	CameraWidget::CameraWidget(AIngine::Rendering::Camera & camera)
		: m_camera(camera)
	{
	}


	CameraWidget::~CameraWidget()
	{
	}


	void CameraWidget::OnImGuiRender()
	{
		static float dragspeed = 0.1f;
		static glm::vec2 initialPosition(m_camera.GetPosition());
		static float initialZoom(m_camera.m_zoom);
		static float initialRot(m_camera.m_rotation);

		if (ImGui::Begin("Camera"))
		{
			m_size = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

			// zoom
			ImGui::DragFloat("Zoom", &m_camera.m_zoom, dragspeed, m_camera.GetZoomMin(), m_camera.GetZoomMax());
			// position
			float* pos[] = { &m_camera.m_position.x, &m_camera.m_position.y };
			ImGui::DragFloat2("Position", *pos);
			//rotation
			ImGui::DragFloat("Rotation", &m_camera.m_rotation, dragspeed, -M_PI, M_PI);

			ImGui::Separator();

			// reset camera button
			if (ImGui::Button("Reset")) {
				m_camera.SetPosition(initialPosition);
				m_camera.SetZoom(initialZoom);
				m_camera.SetRotation(initialRot);
			}

			ImGui::Separator();

			// look at
			static glm::vec2 lookpos(0);
			static float* lookposvalues[] = { &lookpos.x, &lookpos.y };

			ImGui::DragFloat2("Look Position", *lookposvalues);
			if (ImGui::Button("Look At")) {
				m_camera.LookAt(lookpos);
			}

			ImGui::Separator();

			glm::vec2 screenPos = glm::vec2(AIngine::Input::GetMouseX(), AIngine::Input::GetMouseY());
			glm::vec2 worldPos = m_camera.ScreenToWorldPoint(screenPos);

			ImGui::Text("World Position (%.2f | %.2f)", worldPos.x, worldPos.y);

			ImGui::End();
		}
	}
}
