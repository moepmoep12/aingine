#pragma once

#include "Editor/Widgets/EditorWidget.h"

namespace AIngine::Editor {
	class MenubarWidget : public EditorWidget {
	public:
		// Inherited via EditorWidget
		virtual void OnImGuiRender() override;

		MenubarWidget();
	};
}