#include "Editor/Widgets/SpriteComponentWidget.h"
#include "AIngine/GameObject.h"
#include "AIngine/Sprite.h"
#include "Rendering/texture.h"
#include "Assets/Assets.h"
#include "Editor/Widgets/Common.h"
#include "Util/Project.h"
#include "Util/FileSystem.h"

#include <map>
#include <filesystem>

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
				float* color[] = { &spriteComponent->m_color.x,&spriteComponent->m_color.y ,&spriteComponent->m_color.z, &spriteComponent->m_color.w };
				float* parallax[] = { &spriteComponent->m_parallaxFactor.x, &spriteComponent->m_parallaxFactor.y };

				Widgets::DisplayTitle(spriteComponent, "Sprite Component");
				ImGui::NewLine();

				// WorldSize
				ImGui::Columns(1);
				ImGui::DragFloat2("WorldSize", *size, dragSpeed, 0.0f, 1000.0f);

				// Color
				ImGui::Indent();
				ImGui::ColorEdit4("Color", *color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
				ImGui::Unindent();
				ImGui::DragFloat2("Parallax Factor", *parallax, 0.02f, -100.0f, 100.0f);

				// Preview Image
				Widgets::PreviewImage(spriteComponent->GetTexture());

				// load new texture
				if (ImGui::IsItemClicked())
				{
					static const char *filterList = "png,jpg,jpeg,bmp";
					std::string path;
					AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::OpenFile(filterList, &path, "textures");

					if (result == AIngine::Util::Filesystem::OKAY)
					{
						AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
						spriteComponent->m_texture.Generate(bitmap);
					}
				}

				ImGui::NewLine();
				ImGui::NewLine();

				// Texture params
				Widgets::TextureParams(spriteComponent->GetTexture());

				// Apply changes to generate a new spriteComponent
				if (ImGui::Button("Apply Changes"))
				{
					AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(spriteComponent->GetTexture().FileName)->GetBitmap();
					spriteComponent->m_texture.Generate(bitmap);
				}
				ImGui::Separator();

			}
		}
	}
}
