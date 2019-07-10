#include "Editor/Widgets/CameraWidget.h"
#include "AIngine/Constants.h"
#include "AIngine/Input.h"
#include "Application.h"
#include "Rendering/Viewport.h"

#include <glm/glm.hpp>

namespace AIngine::Editor::Widget {
	CameraWidget::CameraWidget(AIngine::Rendering::Camera & camera)
		: m_camera(camera), m_initialZoom(camera.m_zoom), m_initialPosition(camera.GetPosition()), m_initialRot(camera.GetRotation())
	{
	}


	CameraWidget::~CameraWidget()
	{
	}


	void CameraWidget::OnImGuiRender()
	{
		static float dragspeed = 0.1f;;

		if (ImGui::Begin("Camera"))
		{
			m_size = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
			glm::vec2 newPosition = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
			m_wasSizeChanged = newPosition != m_position;
			m_position = newPosition;
			m_isDocked = ImGui::IsWindowDocked();

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
				m_camera.SetPosition(m_initialPosition);
				m_camera.SetZoom(m_initialZoom);
				m_camera.SetRotation(m_initialRot);
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
			glm::vec2 WorldToScreen = m_camera.WorldToScreenPoint(worldPos);

			ImGui::Text("MousePosition (%.2f | %.2f)", screenPos.x, screenPos.y);
			ImGui::Text("Screen To World (%.2f | %.2f)", worldPos.x, worldPos.y);
			ImGui::Text("World To Screen (%.2f | %.2f)", WorldToScreen.x, WorldToScreen.y);
			ImGui::Text("Viewport Size (%i | %i)", AIngine::Application::GetViewport().GetViewportWidth(), AIngine::Application::GetViewport().GetViewportHeight());

			float deltaTime = AIngine::Application::Get().GetDeltaTimeReal();

			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("%.3f ms/frame (%.1f FPS)", deltaTime * 1000.0f, 1.0f / deltaTime);

			//ImGui::DragFloat2()

		}
		ImGui::End();
	}
}
