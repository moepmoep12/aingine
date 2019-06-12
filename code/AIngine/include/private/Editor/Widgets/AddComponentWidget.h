#pragma once

#include "Editor/Widgets/ComponentWidget.h"


namespace AIngine::Editor {

	class AddComponentWidget : public ComponentWidget {
	public:
		AddComponentWidget();
		virtual ~AddComponentWidget();
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;
	};
}