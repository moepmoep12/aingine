#pragma once
#include "Editor/Widgets/ComponentWidget.h"

namespace AIngine::Editor::Widget::Component {
	class AgentSupervisorComponentWidget : public ComponentWidget {
	public:
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;
	};

	class AgentComponentWidget : public ComponentWidget {
	public:
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;
	};
}