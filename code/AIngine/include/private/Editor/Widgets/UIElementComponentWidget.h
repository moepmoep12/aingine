#pragma once
#include "UI/UIELement.h"

namespace AIngine::Editor::Widget::Component {

	bool AnchorCombo(AIngine::UI::UIElement* element);

	bool ChangeTransform(AIngine::UI::UIElement* element);

	bool ColorEdit(float* Color, const char* title);

}