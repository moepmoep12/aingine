#include "Editor/Widgets/SliderComponentWidget.h"
#include "UI/Slider.h"
#include "Editor/Widgets/Common.h"
#include "Editor/Widgets/UIElementComponentWidget.h"
#include "AIngine/Graphics.h"
#include "Util/FileSystem.h"
#include "Assets/Assets.h"

namespace AIngine::Editor::Widget::Component {
	void SliderComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			using Slider = AIngine::UI::Slider;
			Slider* checkbox = obj->GetComponent<Slider>();
			if (checkbox) {

				DisplayTitle(checkbox, "Slider");

				ImGui::NewLine();

				ChangeTransform(checkbox);

				if (AnchorCombo(checkbox)) {
					checkbox->m_sliderHandle->SetAnchor(checkbox->GetAnchor());
				}

				ImGui::NewLine();

				// Color TintBackGround
				std::string label = "Tint Color Area##checkboxcomp";
				float* tintColorBackGround[] = { &checkbox->TintColor.x,&checkbox->TintColor.y,&checkbox->TintColor.z, &checkbox->TintColor.w };
				ImGui::ColorEdit4(label.c_str(), *tintColorBackGround, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
				// Color Tint Handle
				label = "Tint Color Handle##checkboxcomp";
				float* tintColorHandle[] = { &checkbox->m_sliderHandle->TintColor.x,&checkbox->m_sliderHandle->TintColor.y,&checkbox->m_sliderHandle->TintColor.z, &checkbox->m_sliderHandle->TintColor.w };
				ImGui::ColorEdit4(label.c_str(), *tintColorHandle, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
				// Color Dragging Handle
				label = "Dragging Color Handle##checkboxcomp";
				float* dragColor[] = { &checkbox->m_sliderHandle->DragColor.x,&checkbox->m_sliderHandle->DragColor.y,&checkbox->m_sliderHandle->DragColor.z, &checkbox->m_sliderHandle->DragColor.w };
				ImGui::ColorEdit4(label.c_str(), *dragColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);

				// BackGround Texture
				PreviewImage(checkbox->TextureBackGround);
				if (ImGui::IsItemClicked())
				{
					static const char *filterList = "png,jpg,jpeg,bmp";
					std::string path;
					AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::OpenFile(filterList, &path, "textures");

					if (result == AIngine::Util::Filesystem::OKAY)
					{
						AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
						checkbox->TextureBackGround = AIngine::Rendering::Texture2D(bitmap);
					}
				}

				ImGui::NewLine();

				// Handle Texture
				PreviewImage(checkbox->m_sliderHandle->TextureSlider);
				if (ImGui::IsItemClicked())
				{
					static const char *filterList = "png,jpg,jpeg,bmp";
					std::string path;
					AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::OpenFile(filterList, &path, "textures");

					if (result == AIngine::Util::Filesystem::OKAY)
					{
						AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
						checkbox->m_sliderHandle->TextureSlider = AIngine::Rendering::Texture2D(bitmap);
					}
				}

				ImGui::NewLine();

				float value = checkbox->Value;
				if (ImGui::SliderFloat("Value##slider", &value, 0, 1, "%.1f")) {
					checkbox->Value = value;
				}
				float minvalue = checkbox->Min;
				if (ImGui::InputFloat("MinValue", &minvalue)) {
					checkbox->Min = minvalue;
				}
				float maxvalue = checkbox->Max;
				if (ImGui::InputFloat("MaxValue", &maxvalue)) {
					checkbox->Max = maxvalue;
				}

				ImGui::NewLine();
				ImGui::Separator();

				// Draw SliderBackground rect
				AIngine::Graphics::BoxScreen(checkbox->GetRectangle(), glm::vec3(1, 1, 0));
				// Draw SliderHandle rect
				AIngine::Graphics::BoxScreen(checkbox->m_sliderHandle->GetRectangle(), glm::vec3(1, 1, 0));
			}
		}
	}
}