#pragma once

#include "Editor/Widgets/ComponentWidget.h"

namespace AIngine::Editor::Widget::Component {

	class SpriteComponentWidget : public ComponentWidget {
	public:
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;
	};
}