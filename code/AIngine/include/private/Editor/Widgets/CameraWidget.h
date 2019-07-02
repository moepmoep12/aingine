#pragma once

#include "Editor/Widgets/EditorWidget.h"
#include "Rendering/Camera.h"

namespace AIngine::Editor::Widget {
	class CameraWidget : public EditorWidget {

	public:
		CameraWidget(AIngine::Rendering::Camera& camera);
		virtual ~CameraWidget() override;

		// Inherited via EditorWidget
		virtual void OnImGuiRender() override;

	private:
		AIngine::Rendering::Camera& m_camera;
		float m_initialZoom, m_initialRot;
		glm::vec2 m_initialPosition;
	};
}