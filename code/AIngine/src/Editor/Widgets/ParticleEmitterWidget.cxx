#include "Editor/Widgets/ParticleEmitterWidget.h"
#include "AIngine/ParticleEmitter.h"
#include "Assets/Assets.h"
#include "Util/Project.h"
#include "Util/FileSystem.h"

#include <imgui.h>

namespace AIngine::Editor::Widget::Component {
	void ParticleEmitterWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			AIngine::ParticleEmitter* emitter = obj->GetComponent<AIngine::ParticleEmitter>();
			if (emitter) {
				ImGui::Separator();

				// Title
				float textWidth = ImGui::CalcTextSize("Particle Emitter").x;
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) * 0.5f);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Paticle Emitter");
				// ContextMenu
				if (ImGui::BeginPopupContextItem("Particle Emitter")) {
					std::string action = emitter->IsActive() ? "Deactivate" : "Activate";
					action.append("##emitter");
					if (ImGui::Selectable(action.c_str())) {
						emitter->SetEnabled(!emitter->IsActive());
					}
					if (ImGui::Selectable("Delete##emitter")) {
						obj->RemoveComponent<AIngine::ParticleEmitter>();
						ImGui::EndPopup();
						return;
					}
					ImGui::EndPopup();
				}
				ImGui::NewLine();

				int amount = emitter->GetAmount();
				if(ImGui::InputInt("Particle Amount", &amount)) 
				{
					emitter->SetAmount(amount);
				}
				ImGui::NewLine();


				// preview image
				// we preserve the image ratio in the preview
				static ImVec2 uv0 = ImVec2(0, 0);
				static ImVec2 uv1 = ImVec2(1, 1);
				ImVec4 tint_col = ImVec4(1, 1, 1, 1);
				static ImVec4 border_col = ImVec4(1, 1, 1, 1);
				ImVec2 textureSize = ImVec2((float)emitter->GetTexture().Width, (float)emitter->GetTexture().Height);
				float imageRatio = 1;
				if (emitter->GetTexture().FileName.empty()) {
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
				ImGui::Image((ImTextureID)emitter->GetTexture().ID, previewImageSize, uv0, uv1, tint_col, border_col);

				// load new texture
				if (ImGui::IsItemClicked())
				{
					static const char* filterList = "png,jpg,jpeg,bmp";
					std::string path;
					AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::OpenFile(filterList, &path,"textures");

					if (result == AIngine::Util::Filesystem::Result::OKAY)
					{
						AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
						emitter->SetTexture(AIngine::Rendering::Texture2D(bitmap));
					}
				}

				ImGui::NewLine();
				ImGui::NewLine();

				// Texture Dimensions Text
				ImGui::Columns(2);
				ImGui::Text("%ix%i", emitter->GetTexture().Width, emitter->GetTexture().Height);
				ImGui::NextColumn();
				ImGui::Text("Texture Size");

				// Texture Format Colored Text
				ImGui::NextColumn();
				const char* internalFormat = emitter->GetTexture().Image_Format == GL_RGBA ? "GL_RGBA" : "GL_RGB";
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
				static int currentWrapSIndex = ModeToIndex[emitter->GetTexture().Wrap_S];
				if (ImGui::BeginCombo("Wrap S", wrapSNames[currentWrapSIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(wrapSNames); i++) {
						bool isSelected = choosableWrappingModes[i] == emitter->GetTexture().Wrap_S;
						if (ImGui::Selectable(wrapSNames[i], isSelected)) {
							emitter->GetTexture().Wrap_S = choosableWrappingModes[i];
							currentWrapSIndex = i;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}


				// Wrap_T Combo
				static int currentWrapTIndex = ModeToIndex[emitter->GetTexture().Wrap_T];
				if (ImGui::BeginCombo("Wrap T", wrapTNames[currentWrapTIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(wrapTNames); i++) {
						bool isSelected = choosableWrappingModes[i] == emitter->GetTexture().Wrap_T;
						if (ImGui::Selectable(wrapTNames[i], isSelected)) {
							emitter->GetTexture().Wrap_T = choosableWrappingModes[i];
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
				static int filterMinIndex = FilterToIndex[emitter->GetTexture().Filter_Min];
				if (ImGui::BeginCombo("Filter Min", FilterNames[filterMinIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(FilterNames); i++) {
						bool isSelected = FilterOptions[i] == emitter->GetTexture().Filter_Min;
						if (ImGui::Selectable(FilterNames[i], isSelected)) {
							emitter->GetTexture().Filter_Min = FilterOptions[i];
							filterMinIndex = i;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				// Filter max Combo
				static int filterMaxIndex = FilterToIndex[emitter->GetTexture().Filter_Max];
				if (ImGui::BeginCombo("Filter Max", FilterNames[filterMaxIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(FilterNames); i++) {
						bool isSelected = FilterOptions[i] == emitter->GetTexture().Filter_Max;
						if (ImGui::Selectable(FilterNames[i], isSelected)) {
							emitter->GetTexture().Filter_Max = FilterOptions[i];
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
					AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(emitter->GetTexture().FileName)->GetBitmap();

					emitter->GetTexture().Wrap_S = choosableWrappingModes[currentWrapSIndex];
					emitter->GetTexture().Wrap_T = choosableWrappingModes[currentWrapTIndex];
					emitter->GetTexture().Filter_Max = FilterOptions[filterMaxIndex];
					emitter->GetTexture().Filter_Min = FilterOptions[filterMinIndex];

					emitter->GetTexture().Generate(bitmap);
				}
				ImGui::Separator();
			}
		}
	}
}