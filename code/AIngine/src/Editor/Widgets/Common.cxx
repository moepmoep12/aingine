#include "Editor/Widgets/Common.h"
#include "Rendering/texture.h"

#include <map>

namespace AIngine::Editor::Widget {

	void PreviewImage(const AIngine::Rendering::Texture2D & texture)
	{
		// we preserve the image ratio in the preview
		static ImVec2 uv0 = ImVec2(0, 0);
		static ImVec2 uv1 = ImVec2(1, 1);
		ImVec4 tint_col = ImVec4(1, 1, 1, 1);
		static ImVec4 border_col = ImVec4(1, 1, 1, 1);
		ImVec2 textureSize = ImVec2((float)texture.Width, (float)texture.Height);
		float imageRatio = 1;
		if (texture.FileName.empty()) {
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
		ImGui::Image((ImTextureID)texture.ID, previewImageSize, uv0, uv1, tint_col, border_col);
	}

	void TextureParams(AIngine::Rendering::Texture2D & texture)
	{
		// Texture Dimensions Text
		ImGui::Columns(2);
		ImGui::Text("%ix%i", texture.Width, texture.Height);
		ImGui::NextColumn();
		ImGui::Text("Texture Size");

		// Texture Format Colored Text
		ImGui::NextColumn();
		const char* internalFormat = texture.Image_Format == GL_RGBA ? "GL_RGBA" : "GL_RGB";
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
		static int currentWrapSIndex = ModeToIndex[texture.Wrap_S];
		if (ImGui::BeginCombo("Wrap S", wrapSNames[currentWrapSIndex])) {
			for (int i = 0; i < IM_ARRAYSIZE(wrapSNames); i++) {
				bool isSelected = choosableWrappingModes[i] == texture.Wrap_S;
				if (ImGui::Selectable(wrapSNames[i], isSelected)) {
					texture.Wrap_S = choosableWrappingModes[i];
					currentWrapSIndex = i;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}


		// Wrap_T Combo
		static int currentWrapTIndex = ModeToIndex[texture.Wrap_T];
		if (ImGui::BeginCombo("Wrap T", wrapTNames[currentWrapTIndex])) {
			for (int i = 0; i < IM_ARRAYSIZE(wrapTNames); i++) {
				bool isSelected = choosableWrappingModes[i] == texture.Wrap_T;
				if (ImGui::Selectable(wrapTNames[i], isSelected)) {
					texture.Wrap_T = choosableWrappingModes[i];
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
		static int filterMinIndex = FilterToIndex[texture.Filter_Min];
		if (ImGui::BeginCombo("Filter Min", FilterNames[filterMinIndex])) {
			for (int i = 0; i < IM_ARRAYSIZE(FilterNames); i++) {
				bool isSelected = FilterOptions[i] == texture.Filter_Min;
				if (ImGui::Selectable(FilterNames[i], isSelected)) {
					texture.Filter_Min = FilterOptions[i];
					filterMinIndex = i;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		// Filter max Combo
		static int filterMaxIndex = FilterToIndex[texture.Filter_Max];
		if (ImGui::BeginCombo("Filter Max", FilterNames[filterMaxIndex])) {
			for (int i = 0; i < IM_ARRAYSIZE(FilterNames); i++) {
				bool isSelected = FilterOptions[i] == texture.Filter_Max;
				if (ImGui::Selectable(FilterNames[i], isSelected)) {
					texture.Filter_Max = FilterOptions[i];
					filterMaxIndex = i;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}
}