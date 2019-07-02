#pragma once
#include "UI/UIELement.h"

namespace AIngine::Editor {
	class UIElementComponentWidget {
	protected:
		virtual void ChangeTransform(AIngine::UI::UIElement* element);
		virtual void ColorEdit(float* Color, const char* title);

		virtual void RemoveComponent() = 0;
	};
}