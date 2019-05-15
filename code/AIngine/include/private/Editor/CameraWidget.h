#pragma once

#include "Editor/ImGuiWidget.h"
#include "Rendering/Camera.h"

namespace AIngine::Editor {
	class CameraWidget : public ImGuiWidget {

	public:
		CameraWidget(AIngine::Rendering::Camera& camera);
		virtual ~CameraWidget() override;

		// Inherited via ImGuiWidget
		virtual void OnImGuiRender() override;

	private:
		AIngine::Rendering::Camera& m_camera;

	};
}