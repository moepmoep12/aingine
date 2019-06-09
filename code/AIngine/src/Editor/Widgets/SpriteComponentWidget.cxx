#include "Editor/Widgets/SpriteComponentWidget.h"
#include "AIngine/GameObject.h"
#include "AIngine/Sprite.h"
#include "Rendering/texture.h"
#include "Assets/Assets.h"

namespace AIngine::Editor {

	void AIngine::Editor::SpriteComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			AIngine::Sprite* spriteComponent = obj->GetComponent<AIngine::Sprite>();
			if (spriteComponent) {
				ImGui::Separator();
				static float dragSpeed = 0.1f;
				float* size[] = { &spriteComponent->m_localWorldSize.x ,&spriteComponent->m_localWorldSize.y };
				float* color[] = { &spriteComponent->m_color.x,&spriteComponent->m_color.y ,&spriteComponent->m_color.z };
				float* parallax[] = { &spriteComponent->m_parallaxFactor.x, &spriteComponent->m_parallaxFactor.y };

				// Title
				ImGui::BulletText("Sprite Component");
				if (ImGui::BeginPopupContextItem("Sprite Component")) {
					if (ImGui::Selectable("Delete##spriteComp")) {
						obj->RemoveComponent<AIngine::Sprite>();
						ImGui::EndPopup();
						return;
					}
					ImGui::EndPopup();
				}
				ImGui::NewLine();

				// WorldSize
				ImGui::Columns(1);
				ImGui::DragFloat2("WorldSize", *size, dragSpeed, 0.0f, 1000.0f);

				// Color
				ImGui::Indent();
				ImGui::ColorEdit3("Color", *color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
				ImGui::Unindent();
				ImGui::DragFloat2("Parallax Factor", *parallax, 0.02f, -100.0f, 100.0f);

				// preview image
				// we preserve the image ratio in the preview
				float imageRatio = (float)spriteComponent->m_texture.Height / (float)spriteComponent->m_texture.Width;
				ImGui::NewLine();
				ImGui::NewLine();
				ImGui::Image((ImTextureID)spriteComponent->m_texture.ID, ImVec2(ImGui::GetWindowWidth() * 0.9f, ImGui::GetWindowWidth() * 0.9f * imageRatio), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

				ImGui::NewLine();
				ImGui::NewLine();


				// Texture Dimensions Text
				ImGui::Columns(2);
				ImGui::Text("%ix%i", spriteComponent->m_texture.Width, spriteComponent->m_texture.Height);
				ImGui::NextColumn();
				ImGui::Text("Texture Size");

				// Texture Format Colored Text
				ImGui::NextColumn();
				const char* internalFormat = spriteComponent->m_texture.Image_Format == GL_RGBA ? "GL_RGBA" : "GL_RGB";
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

				ImGui::Columns(1);

				// Wrap_S Combo
				static int currentWrapSIndex = ModeToIndex[spriteComponent->m_texture.Wrap_S];
				if (ImGui::BeginCombo("Wrap S", wrapSNames[currentWrapSIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(wrapSNames); i++) {
						bool isSelected = choosableWrappingModes[i] == spriteComponent->m_texture.Wrap_S;
						if (ImGui::Selectable(wrapSNames[i], isSelected)) {
							spriteComponent->m_texture.Wrap_S = choosableWrappingModes[i];
							currentWrapSIndex = i;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}


				// Wrap_T Combo
				static int currentWrapTIndex = ModeToIndex[spriteComponent->m_texture.Wrap_T];
				if (ImGui::BeginCombo("Wrap T", wrapTNames[currentWrapTIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(wrapTNames); i++) {
						bool isSelected = choosableWrappingModes[i] == spriteComponent->m_texture.Wrap_T;
						if (ImGui::Selectable(wrapTNames[i], isSelected)) {
							spriteComponent->m_texture.Wrap_T = choosableWrappingModes[i];
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
				static int filterMinIndex = FilterToIndex[spriteComponent->m_texture.Filter_Min];
				if (ImGui::BeginCombo("Filter Min", FilterNames[filterMinIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(FilterNames); i++) {
						bool isSelected = FilterOptions[i] == spriteComponent->m_texture.Filter_Min;
						if (ImGui::Selectable(FilterNames[i], isSelected)) {
							spriteComponent->m_texture.Filter_Min = FilterOptions[i];
							filterMinIndex = i;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				// Filter max Combo
				static int filterMaxIndex = FilterToIndex[spriteComponent->m_texture.Filter_Max];
				if (ImGui::BeginCombo("Filter Max", FilterNames[filterMaxIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(FilterNames); i++) {
						bool isSelected = FilterOptions[i] == spriteComponent->m_texture.Filter_Max;
						if (ImGui::Selectable(FilterNames[i], isSelected)) {
							spriteComponent->m_texture.Filter_Max = FilterOptions[i];
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
					AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(spriteComponent->GetTexture().FileName)->GetBitmap();

					spriteComponent->m_texture.Wrap_S = choosableWrappingModes[currentWrapSIndex];
					spriteComponent->m_texture.Wrap_T = choosableWrappingModes[currentWrapTIndex];
					spriteComponent->m_texture.Filter_Max = FilterOptions[filterMaxIndex];
					spriteComponent->m_texture.Filter_Min = FilterOptions[filterMinIndex];

					spriteComponent->m_texture.Generate(bitmap);
				}


			}
		}
	}
}
