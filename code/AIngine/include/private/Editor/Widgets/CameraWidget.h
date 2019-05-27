#pragma once

#include "Editor/Widgets/EditorWidget.h"
#include "Rendering/Camera.h"

namespace AIngine::Editor {
	class CameraWidget : public EditorWidget {

	public:
		CameraWidget(AIngine::Rendering::Camera& camera);
		virtual ~CameraWidget() override;

		// Inherited via EditorWidget
		virtual void OnImGuiRender() override;

	private:
		AIngine::Rendering::Camera& m_camera;

	};
}