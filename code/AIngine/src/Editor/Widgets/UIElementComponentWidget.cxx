#include "Editor/Widgets/UIElementComponentWidget.h"
#include "UI/UIELement.h"

#include <typeinfo>
#include "imgui.h"

namespace AIngine::Editor {

	void UIElementComponentWidget::ChangeTransform(AIngine::UI::UIElement * element)
	{
		int pos[] = { element->GetRectangle().x, element->GetRectangle().y };
		int dim[] = { element->GetRectangle().width, element->GetRectangle().height };
		if (ImGui::DragInt2("Screen Position", pos)) {
			element->SetPosition(glm::vec2(pos[0], pos[1]));
		}
		if (ImGui::DragInt2("Dimensions", dim)) {
			element->SetWidth(dim[0]);
			element->SetHeight(dim[1]);
		}

		// Disabled
		bool isDisabled = element->IsDisabled();
		if (ImGui::Checkbox("Disabled", &isDisabled)) {
			element->SetDisabled(isDisabled);
		}
	}
	void UIElementComponentWidget::ColorEdit(float * Color, const char * title)
	{
		static const ImGuiColorEditFlags flags = ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar;
		ImGui::ColorEdit4(title, Color, flags);
	}
}