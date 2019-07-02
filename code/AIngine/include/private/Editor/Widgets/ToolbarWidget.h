#pragma once
#include "Editor/Widgets/EditorWidget.h"
#include "Rendering/texture.h"

#include <string>
#include <map>

namespace AIngine::Editor::Widget {

	class ToolbarWidget : public EditorWidget {
	public:
		ToolbarWidget();
		virtual ~ToolbarWidget();

		// Inherited via EditorWidget
		virtual void OnImGuiRender() override;

	private:
		std::map<std::string, AIngine::Rendering::Texture2D> m_Icons;


	};
}