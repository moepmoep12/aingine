#pragma once
#include "Editor/Widgets/ComponentWidget.h"

namespace AIngine::Editor::Widget::Component {
	class ButtonComponentWidget : public ComponentWidget {
	public:
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;
	};
}