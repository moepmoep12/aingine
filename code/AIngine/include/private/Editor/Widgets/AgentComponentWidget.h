#pragma once
#include "Editor/Widgets/ComponentWidget.h"
#include "Editor/Editor.h"

namespace AIngine::Editor::Widget::Component {
	class AgentSupervisorComponentWidget : public ComponentWidget {
	public:
		AgentSupervisorComponentWidget();
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;

	private:
		AIngine::Editor::Editor::SelectionChangedEventHandler OnSelectionChangedHandler;

		void OnSelectionChanged(const std::vector<AIngine::GameObject*>& selectedObjects);
	};

	class AgentComponentWidget : public ComponentWidget {
	public:
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;
	};
}