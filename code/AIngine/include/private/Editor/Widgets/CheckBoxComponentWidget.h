#pragma once
#include "Editor/Widgets/ComponentWidget.h"

namespace AIngine::Editor::Widget::Component {
	class CheckBoxComponentWidget : public ComponentWidget {
	public:
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;
	};
}