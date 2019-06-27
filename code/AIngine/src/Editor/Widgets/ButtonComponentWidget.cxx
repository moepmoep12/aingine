#include "Editor/Widgets/ButtonComponentWidget.h"
#include "UI/Button.h"
#include "Assets/Assets.h"
#include "AIngine/Graphics.h"

#include <imgui.h>
#include <nfd.h>

namespace AIngine::Editor {
	void ButtonComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			using Button = AIngine::UI::Button;
			Button* btn = obj->GetComponent<Button>();
			if (btn) {

				// Title
				float textWidth = ImGui::CalcTextSize("Button").x;
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) * 0.5f);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Button");
				// ContextMenu
				if (ImGui::BeginPopupContextItem("Button")) {
					std::string action = btn->IsActive() ? "Deactivate" : "Activate";
					action.append("##button");
					if (ImGui::Selectable(action.c_str())) {
						btn->SetEnabled(!btn->IsActive());
					}
					if (ImGui::Selectable("Delete##button")) {
						obj->RemoveComponent<Button>();
						ImGui::EndPopup();
						return;
					}
					ImGui::EndPopup();
				}

				ImGui::NewLine();

				// Change rect transform
				int* pos[] = { &btn->m_rectangle.x, &btn->m_rectangle.y };
				int* dim[] = { &btn->m_rectangle.width, &btn->m_rectangle.height };
				ImGui::DragInt2("Screen Position", *pos);
				ImGui::DragInt2("Dimensions", *dim);

				// Disabled
				bool isDisabled = btn->IsDisabled();
				if (ImGui::Checkbox("Disabled", &isDisabled)) {
					btn->SetDisabled(isDisabled);
				}

				ImGui::NewLine();

				// Color Tint
				float* tintColor[] = { &btn->TintColor.x,&btn->TintColor.y,&btn->TintColor.z, &btn->TintColor.w };
				ImGui::ColorEdit4("TintColor", *tintColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
				// Color Disabled
				float* disabledColor[] = { &btn->DisabledColor.x,&btn->DisabledColor.y,&btn->DisabledColor.z, &btn->DisabledColor.w };
				ImGui::ColorEdit4("disabledColor", *disabledColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
				// Color Hovered
				float* hoveredColor[] = { &btn->HoverColor.x,&btn->HoverColor.y,&btn->HoverColor.z, &btn->HoverColor.w };
				ImGui::ColorEdit4("hoveredColor", *hoveredColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
				// Color Clicked
				float* clickedColor[] = { &btn->ClickedColor.x,&btn->ClickedColor.y,&btn->ClickedColor.z, &btn->ClickedColor.w };
				ImGui::ColorEdit4("clickedColor", *clickedColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);

				// preview image
					// we preserve the image ratio in the preview
				static ImVec2 uv0 = ImVec2(0, 0);
				static ImVec2 uv1 = ImVec2(1, 1);
				ImVec4 tint_col = ImVec4(1, 1, 1, 1);
				static ImVec4 border_col = ImVec4(1, 1, 1, 1);
				ImVec2 textureSize = ImVec2((float)btn->Texture.Width, (float)btn->Texture.Height);
				float imageRatio = 1;
				if (btn->Texture.FileName.empty()) {
					tint_col = ImVec4(0, 0, 0, 1);
					textureSize.x = 500;
					textureSize.y = 500;
				}

				imageRatio = textureSize.y / textureSize.x;
				ImVec2 previewImageSize = ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, ImGui::GetWindowContentRegionWidth() * 0.5f * imageRatio);

				ImGui::NewLine();
				ImGui::NewLine();
				// create the image preview
				ImGui::SetCursorPosX(0.5f * (ImGui::GetWindowContentRegionWidth() - previewImageSize.x));
				ImGui::Image((ImTextureID)btn->Texture.ID, previewImageSize, uv0, uv1, tint_col, border_col);

				// load new texture
				if (ImGui::IsItemClicked())
				{
					static const nfdchar_t *filterList = "png,jpg,jpeg,bmp";
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialog(filterList, NULL, &outPath);

					if (result == NFD_OKAY)
					{
						AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(outPath)->GetBitmap();
						btn->Texture = AIngine::Rendering::Texture2D(bitmap);
						free(outPath);
					}
				}

				ImGui::NewLine();
				ImGui::NewLine();

				// Texture Dimensions Text
				ImGui::Columns(2);
				ImGui::Text("%ix%i", btn->Texture.Width, btn->Texture.Height);
				ImGui::NextColumn();
				ImGui::Text("Texture Size");

				// Texture Format Colored Text
				ImGui::NextColumn();
				const char* internalFormat = btn->Texture.Image_Format == GL_RGBA ? "GL_RGBA" : "GL_RGB";
				ImGui::TextColored(ImVec4(0.5, 0, 0.5, 1), internalFormat);
				ImGui::NextColumn();
				ImGui::Text("Texture Format");

				// Wrapping
				static const GLint choosableWrappingModes[] = { GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER };
				static std::map<GLint, int> ModeToIndex;
				ModeToIndex[GL_REPEAT] = 0;
				ModeToIndex[GL_MIRRORED_REPEAT] = 1;
				ModeToIndex[GL_CLAMP_TO_EDGE] = 2;
				ModeToIndex[GL_CLAMP_TO_BORDER] = 3;
				static const char* wrapSNames[] = { "Repeat","Mirrored Repeat", "Clamp to Edge", "Clamp to Border" };
				static const char* wrapTNames[] = { "Repeat","Mirrored Repeat", "Clamp to Edge", "Clamp to Border" };

				ImGui::EndColumns();

				// Wrap_S Combo
				static int currentWrapSIndex = ModeToIndex[btn->Texture.Wrap_S];
				if (ImGui::BeginCombo("Wrap S", wrapSNames[currentWrapSIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(wrapSNames); i++) {
						bool isSelected = choosableWrappingModes[i] == btn->Texture.Wrap_S;
						if (ImGui::Selectable(wrapSNames[i], isSelected)) {
							btn->Texture.Wrap_S = choosableWrappingModes[i];
							currentWrapSIndex = i;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}


				// Wrap_T Combo
				static int currentWrapTIndex = ModeToIndex[btn->Texture.Wrap_T];
				if (ImGui::BeginCombo("Wrap T", wrapTNames[currentWrapTIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(wrapTNames); i++) {
						bool isSelected = choosableWrappingModes[i] == btn->Texture.Wrap_T;
						if (ImGui::Selectable(wrapTNames[i], isSelected)) {
							btn->Texture.Wrap_T = choosableWrappingModes[i];
							currentWrapTIndex = i;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				// Filter
				static const GLint FilterOptions[] = { GL_NEAREST , GL_LINEAR };
				static std::map<GLint, int> FilterToIndex;
				FilterToIndex[GL_NEAREST] = 0;
				FilterToIndex[GL_LINEAR] = 1;
				static const char* FilterNames[] = { "Nearest" , "Linear" };

				// Filter min Combo
				static int filterMinIndex = FilterToIndex[btn->Texture.Filter_Min];
				if (ImGui::BeginCombo("Filter Min", FilterNames[filterMinIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(FilterNames); i++) {
						bool isSelected = FilterOptions[i] == btn->Texture.Filter_Min;
						if (ImGui::Selectable(FilterNames[i], isSelected)) {
							btn->Texture.Filter_Min = FilterOptions[i];
							filterMinIndex = i;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				// Filter max Combo
				static int filterMaxIndex = FilterToIndex[btn->Texture.Filter_Max];
				if (ImGui::BeginCombo("Filter Max", FilterNames[filterMaxIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(FilterNames); i++) {
						bool isSelected = FilterOptions[i] == btn->Texture.Filter_Max;
						if (ImGui::Selectable(FilterNames[i], isSelected)) {
							btn->Texture.Filter_Max = FilterOptions[i];
							filterMaxIndex = i;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				// Apply changes to generate a new spriteComponent
				if (ImGui::Button("Apply Changes"))
				{
					AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(btn->Texture.FileName)->GetBitmap();

					btn->Texture.Wrap_S = choosableWrappingModes[currentWrapSIndex];
					btn->Texture.Wrap_T = choosableWrappingModes[currentWrapTIndex];
					btn->Texture.Filter_Max = FilterOptions[filterMaxIndex];
					btn->Texture.Filter_Min = FilterOptions[filterMinIndex];

					btn->Texture.Generate(bitmap);
				}

				ImGui::NewLine();
				ImGui::Separator();

				// Draw button rect
				AIngine::Graphics::BoxScreen(btn->m_rectangle, glm::vec3(1, 1, 0));
			}
		}
	}
}