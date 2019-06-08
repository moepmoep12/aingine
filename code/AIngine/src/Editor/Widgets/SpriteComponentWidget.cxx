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
				ImGui::BulletText("Texture Component");
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


				// Texture Dimensions
				ImGui::Columns(2);
				ImGui::Text("%ix%i", spriteComponent->m_texture.Width, spriteComponent->m_texture.Height);
				ImGui::NextColumn();
				ImGui::Text("Texture Size");

				// Texture Format
				ImGui::NextColumn();
				const char* internalFormat = spriteComponent->m_texture.Image_Format == GL_RGBA ? "GL_RGBA" : "GL_RGB";
				ImGui::TextColored(ImVec4(0.5, 0, 0.5, 1), internalFormat);
				ImGui::NextColumn();
				ImGui::Text("Texture Format");

				// Wrapping
				static const GLint choosableWrappingModes[] = { GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER };
				static const char* wrapSNames[] = { "Repeat","Mirrored Repeat", "Clamp to Edge", "Clamp to Border" };
				static const char* wrapTNames[] = { "Repeat","Mirrored Repeat", "Clamp to Edge", "Clamp to Border" };
				struct FuncHolder { static bool ItemGetter(void* data, int idx, const char** out_str) { *out_str = ((const char**)data)[idx]; return true; } };

				// Wrap_S
				static int currentWrapS = 0;
				ImGui::NextColumn();
				ImGui::Combo("", &currentWrapS, &FuncHolder::ItemGetter, wrapSNames, IM_ARRAYSIZE(wrapSNames));
				ImGui::NextColumn();
				ImGui::Text("Wrap S");

				// Wrap_T
				static int currentWrapT = 0;
				ImGui::NextColumn();
				ImGui::Combo("", &currentWrapT, &FuncHolder::ItemGetter, wrapTNames, IM_ARRAYSIZE(wrapTNames));
				ImGui::NextColumn();
				ImGui::Text("Wrap T");

				// Filter
				static const GLint FilterOptions[] = { GL_NEAREST , GL_LINEAR };
				static const char* FilterNames[] = { "Nearest" , "Linear" };

				// Filter min
				static int filterMinIndex = 1;
				ImGui::NextColumn();
				ImGui::Combo("", &filterMinIndex, &FuncHolder::ItemGetter, FilterNames, IM_ARRAYSIZE(FilterNames));
				ImGui::NextColumn();
				ImGui::Text("Filter min");

				// Filter max
				static int filterMaxIndex = 1;
				ImGui::NextColumn();
				ImGui::Combo("", &filterMaxIndex, &FuncHolder::ItemGetter, FilterNames, IM_ARRAYSIZE(FilterNames));
				ImGui::NextColumn();
				ImGui::Text("Filter max");

				ImGui::Columns(1);
				ImGui::NewLine();
				ImGui::NewLine();

				// Apply changes to generate a new spriteComponent
				if (ImGui::Button("Apply Changes"))
				{
					AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(spriteComponent->GetName())->GetBitmap();

					spriteComponent->m_texture.Wrap_S = choosableWrappingModes[currentWrapS];
					spriteComponent->m_texture.Wrap_T = choosableWrappingModes[currentWrapT];
					spriteComponent->m_texture.Filter_Max = FilterOptions[filterMinIndex];
					spriteComponent->m_texture.Filter_Min = FilterOptions[filterMaxIndex];

					spriteComponent->m_texture.Generate(bitmap);
				}


			}
		}
	}
}
