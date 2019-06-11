#include "Editor/Widgets/ToolbarWidget.h"
#include "Assets/Assets.h"
#include "Editor/Editor.h"
#include "Editor/Serialization.h"

#include <nfd.h>

namespace AIngine::Editor {

	ToolbarWidget::ToolbarWidget()
	{
		// load icons

		// playIcon
		std::string path = "assets/Intellgine/textures/Editor/play.png";
		AIngine::Rendering::Bitmap* bitmap = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
		m_Icons["play"] = AIngine::Rendering::Texture2D(*bitmap);

		// pauseIcon
		path = "assets/Intellgine/textures/Editor/pause.png";
		bitmap = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
		m_Icons["pause"] = AIngine::Rendering::Texture2D(*bitmap);

		// openIcon
		path = "assets/Intellgine/textures/Editor/open.png";
		bitmap = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(path)->GetBitmap();
		m_Icons["open"] = AIngine::Rendering::Texture2D(*bitmap);
	}

	ToolbarWidget::~ToolbarWidget()
	{
		m_Icons.clear();
	}

	void ToolbarWidget::OnImGuiRender()
	{
		static bool p_open = true;
		static ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration /*| ImGuiWindowFlags_NoInputs*/;

		if (!ImGui::Begin("Toolbar", &p_open, windowFlags))
		{
			ImGui::End();
			return;
		}

		m_size = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		glm::vec2 pos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		m_wasSizeChanged = pos != m_position;
		m_position = pos;
		m_isDocked = ImGui::IsWindowDocked();


		ImVec2 buttonSize = ImVec2(48, 48);
		static const ImVec2 uv0 = ImVec2(0, 0);
		static const ImVec2 uv1 = ImVec2(1, 1);
		static const int framePadding = 0;
		static const ImVec4 backgroundColor = ImVec4(0, 0, 0, 0);
		static const ImVec4 tintColor = ImVec4(1, 1, 1, 1);

		ImGui::Columns(1);

		bool isInPlayMode = AIngine::Editor::Editor::GetIsInPlayMode();
		std::string icon = isInPlayMode ? "pause" : "play";

		if (ImGui::ImageButton((ImTextureID)m_Icons[icon].ID, buttonSize, uv0, uv1, framePadding, backgroundColor, tintColor))
		{
			AIngine::Editor::Editor::SetIsInPlayMode(!isInPlayMode);
		}

		if (ImGui::ImageButton((ImTextureID)m_Icons["open"].ID, buttonSize, uv0, uv1, framePadding, backgroundColor, tintColor))
		{
			static const nfdchar_t *filterList = "txt,json";
			nfdchar_t *outPath = NULL;
			nfdresult_t result = NFD_OpenDialog(filterList, NULL, &outPath);

			if (result == NFD_OKAY)
			{
				AIngine::Editor::Editor::SetCurrentSceneFilePath(std::string(outPath));
				// delete the old tree
				AIngine::Editor::Editor::ResetSceneGraph();
				AIngine::Editor::Serialization::Serializer::DeserializeSceneGraph(AIngine::Editor::Editor::GetCurrentSceneFilePath());
				free(outPath);
			}

			ImGui::End();
		}
	}

}