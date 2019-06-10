#pragma once

#include "Editor/Widgets/ComponentWidget.h"

namespace AIngine::Editor {

	class AddComponentWidget : public ComponentWidget {
	public:
		// Inherited via ComponentWidget
		virtual void OnImGuiRender() override;

	};
}