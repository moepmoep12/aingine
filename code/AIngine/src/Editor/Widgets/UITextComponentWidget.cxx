#include "Editor/Widgets/UITextComponentWidget.h"
#include "Editor/Widgets/UIElementComponentWidget.h"
#include "Editor/Widgets/Common.h"
#include "AIngine/Graphics.h"
#include "UI/Text.h"
#include "Util/FileSystem.h"
#include "Assets/Assets.h"

#include <imgui.h>

namespace AIngine::Editor::Widget::Component {
	void UITextComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			using Text = AIngine::UI::UIText;

			Text* txt = obj->GetComponent<Text>();
			if (txt) {

				DisplayTitle(txt, "Text");

				ImGui::NewLine();

				ChangeTransform(txt);

				AnchorCombo(txt);

				ImGui::NewLine();

				// Color Tint
				std::string label = "Tint Color##textTint";
				float* tintColor[] = { &txt->TintColor.x,&txt->TintColor.y,&txt->TintColor.z, &txt->TintColor.w };
				ImGui::ColorEdit4(label.c_str(), *tintColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);

				// Text
				label = "Text##textcomp";
				static char str0[40] ="";
				if (ImGui::InputText(label.c_str(), str0, IM_ARRAYSIZE(str0))) {
					txt->Text = str0;
				}

				// Font
				ImGui::Text(txt->GetFont().Name.c_str());
				if (ImGui::IsItemClicked()) {
					static const char *filterList = "ttf";
					std::string path;
					AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::OpenFile(filterList, &path, "fonts");

					if (result == AIngine::Util::Filesystem::OKAY)
					{
						std::string p = AIngine::Assets::GetFontPath(path, 50);
						txt->SetFont(&AIngine::Assets::AssetRegistry::Load<AIngine::Assets::FontAsset>(p)->GetFont());
					}
				}

				ImGui::NewLine();

				// FontSize
				int size = static_cast<int>(txt->GetFontSize());
				if (ImGui::DragInt("FontSize##uitext", &size, 1, 1, 200)) {
					txt->ChangeFontSize(static_cast<unsigned int>(size));
				}


				ImGui::NewLine();
				ImGui::Separator();

				// Draw Text rect
				AIngine::Graphics::BoxScreen(txt->GetRectangle(), glm::vec3(1, 1, 0));
			}
		}
	}
}