#include "Editor/Widgets/ImageComponentWidget.h"
#include "UI/Image.h"
#include "Editor/Widgets/Common.h"
#include "Editor/Widgets/UIElementComponentWidget.h"
#include "Util/FileSystem.h"
#include "Assets/Assets.h"
#include "AIngine/Graphics.h"

#include <imgui.h>

namespace AIngine::Editor::Widget::Component {

	void ImageComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			AIngine::GameObject* obj = m_activeGameObjects[0];
			using Image = AIngine::UI::Image;
			Image* img = obj->GetComponent<Image>();
			if (img) {

				if (!DisplayTitle(img, "Image"))
					return;

				ImGui::NewLine();

				ChangeTransform(img);

				AnchorCombo(img);

				ImGui::NewLine();

				// Color Tint
				float* tintColor[] = { &img->TintColor.x,&img->TintColor.y,&img->TintColor.z, &img->TintColor.w };
				ImGui::ColorEdit4("Tint Color", *tintColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);
				// Color Disabled
				float* disabledColor[] = { &img->DisabledColor.x,&img->DisabledColor.y,&img->DisabledColor.z, &img->DisabledColor.w };
				ImGui::ColorEdit4("Disabled Color", *disabledColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar);

				PreviewImage(img->Texture);

				// load new texture
				if (ImGui::IsItemClicked())
				{
					static const char *filterList = "png,jpg,jpeg,bmp";
					std::string path;
					AIngine::Util::Filesystem::Result result = AIngine::Util::Filesystem::OpenFile(filterList, &path, "textures");

					if (result == AIngine::Util::Filesystem::OKAY)
					{
						AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
						img->Texture = AIngine::Rendering::Texture2D(bitmap);
					}
				}

				ImGui::NewLine();
				ImGui::NewLine();

				// Texture Params
				TextureParams(img->Texture);

				// Apply changes to generate a new spriteComponent
				if (ImGui::Button("Apply Changes"))
				{
					AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(img->Texture.FileName)->GetBitmap();
					img->Texture.Generate(bitmap);
				}

				ImGui::NewLine();
				ImGui::Separator();

				// Draw Image rect
				AIngine::Graphics::BoxScreen(img->GetRectangle(), glm::vec4(1, 1, 0, 1));
			}
		}
	}
}