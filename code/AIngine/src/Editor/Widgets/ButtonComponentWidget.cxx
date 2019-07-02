#include "Editor/Widgets/ButtonComponentWidget.h"
#include "UI/Button.h"
#include "Assets/Assets.h"
#include "AIngine/Graphics.h"
#include "Editor/Widgets/Common.h"
#include "Util/FileSystem.h"
#include "Util/Project.h"

#include <imgui.h>

namespace AIngine::Editor {
	void ButtonComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			using Button = AIngine::UI::Button;
			Button* btn = obj->GetComponent<Button>();
			if (btn) {

				AIngine::Editor::Widgets::DisplayTitle(btn, "Button");

				ImGui::NewLine();

				// Change rect transform
				int pos[] = { btn->GetRectangle().x, btn->GetRectangle().y };
				int dim[] = { btn->GetRectangle().width, btn->GetRectangle().height };
				if (ImGui::DragInt2("Screen Position", pos)) {
					btn->SetPosition(glm::vec2(pos[0], pos[1]));
				}
				if (ImGui::DragInt2("Dimensions", dim)) {
					btn->SetWidth(dim[0]);
					btn->SetHeight(dim[1]);
				}

				// Disabled
				bool isDisabled = btn->IsDisabled();
				if (ImGui::Checkbox("Disabled", &isDisabled)) {
					btn->SetDisabled(isDisabled);
				}

				ImGui::NewLine();

				// Color Tint
				float* tintColor[] = { &btn->TintColor.x,&btn->TintColor.y,&btn->TintColor.z, &btn->TintColor.w };
				ImGui::ColorEdit4("Tint Color", *tintColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
				// Color Disabled
				float* disabledColor[] = { &btn->DisabledColor.x,&btn->DisabledColor.y,&btn->DisabledColor.z, &btn->DisabledColor.w };
				ImGui::ColorEdit4("Disabled Color", *disabledColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
				// Color Hovered
				float* hoveredColor[] = { &btn->HoverColor.x,&btn->HoverColor.y,&btn->HoverColor.z, &btn->HoverColor.w };
				ImGui::ColorEdit4("Hovered Color", *hoveredColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
				// Color Clicked
				float* clickedColor[] = { &btn->ClickedColor.x,&btn->ClickedColor.y,&btn->ClickedColor.z, &btn->ClickedColor.w };
				ImGui::ColorEdit4("Clicked Color", *clickedColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
				// Color Text
				float* textColor[] = { &btn->TextColor.x,&btn->TextColor.y,&btn->TextColor.z, &btn->TextColor.w };
				ImGui::ColorEdit4("Text Color", *textColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);

				// Text
				static char str0[40] = "";
				if (ImGui::InputText("Text", str0, IM_ARRAYSIZE(str0))) {
					btn->Text = str0;
				}
				// Text Scale 
				float* scale[] = { &btn->TextScale.x, &btn->TextScale.y };
				ImGui::DragFloat2("Text Scale", *scale, 0.1f);
				// Text Offset
				float* offset[] = { &btn->TextOffset.x, &btn->TextOffset.y };
				ImGui::DragFloat2("Text Offset", *offset);

				AIngine::Editor::Widgets::PreviewImage(btn->Texture);

				// load new texture
				if (ImGui::IsItemClicked())
				{
					static const char *filterList = "png,jpg,jpeg,bmp";
					std::string path;
					AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::OpenFile(filterList, &path,"textures");

					if (result == AIngine::Util::Filesystem::OKAY)
					{
						AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
						btn->Texture = AIngine::Rendering::Texture2D(bitmap);
					}
				}

				ImGui::NewLine();
				ImGui::NewLine();			

				// Texture Params
				AIngine::Editor::Widgets::TextureParams(btn->Texture);

				// Apply changes to generate a new spriteComponent
				if (ImGui::Button("Apply Changes"))
				{
					AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(btn->Texture.FileName)->GetBitmap();
					btn->Texture.Generate(bitmap);
				}

				ImGui::NewLine();
				ImGui::Separator();

				// Draw button rect
				AIngine::Graphics::BoxScreen(btn->GetRectangle(), glm::vec3(1, 1, 0));
			}
		}
	}
}