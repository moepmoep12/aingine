#include "Editor/Widgets/CheckBoxComponentWidget.h"
#include "UI/CheckBox.h"
#include "Editor/Widgets/Common.h"
#include "Editor/Widgets/UIElementComponentWidget.h"
#include "AIngine/Graphics.h"

namespace AIngine::Editor::Widget::Component {

	void CheckBoxComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			using CheckBox = AIngine::UI::CheckBox;
			CheckBox* checkbox = obj->GetComponent<CheckBox>();
			if (checkbox) {

				DisplayTitle(checkbox, "CheckBox");

				ImGui::NewLine();

				ChangeTransform(checkbox);

				if (AnchorCombo(checkbox)) {
					checkbox->TextComponent->SetAnchor(checkbox->GetAnchor());
				}

				ImGui::NewLine();

				// Color Tint
				std::string label = "Tint Color##checkboxcomp";
				float* tintColor[] = { &checkbox->TintColor.x,&checkbox->TintColor.y,&checkbox->TintColor.z, &checkbox->TintColor.w };
				ImGui::ColorEdit4(label.c_str(), *tintColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);

				ImGui::NewLine();

				bool state = checkbox->GetState();
				if (ImGui::Checkbox("State##checkbox", &state)) {
					checkbox->SetState(state);
				}

				ImGui::NewLine();
				ImGui::Separator();

				// Draw CheckBox rect
				AIngine::Graphics::BoxScreen(checkbox->GetRectangle(), glm::vec3(1, 1, 0));
			}
		}
	}
}