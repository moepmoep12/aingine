#include "Editor/Widgets/PopUps.h"
#include "Editor/Editor.h"

#include "imgui.h"
#include <nfd.h>

namespace AIngine::Editor::PopUps {

	void OpenScenePopUpForNewScene()
	{
		ImGui::OpenPopup("SaveSceneForNewScene?");
	}

	void SaveSceneForNewScenePopUp()
	{
		if (ImGui::BeginPopupModal("SaveSceneForNewScene?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Do you want to save changes made to the current scene?");

			std::string currentSceneFilepath = AIngine::Editor::Editor::GetCurrentSceneFilePath();
			bool bSceneLoaded = !currentSceneFilepath.empty();
			static const ImVec2 buttonSize(90, 20);

			if (bSceneLoaded) {
				if (ImGui::Button("Save##saveScenePopUp", buttonSize)) {
					AIngine::Editor::Editor::SaveScene(currentSceneFilepath);
					ImGui::CloseCurrentPopup();
					AIngine::Editor::Editor::CreateNewScene();
				}
			}
			else {
				ImGui::InvisibleButton("Save##saveScenePopUp", buttonSize);
			}

			ImGui::SameLine();

			if (ImGui::Button("Save As##saveScenePopup", buttonSize))
			{
				if (AIngine::Editor::Editor::SaveSceneToFile()) {
					ImGui::CloseCurrentPopup();
					AIngine::Editor::Editor::CreateNewScene();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Don't Save##saveScenePopUp", buttonSize))
			{
				ImGui::CloseCurrentPopup();
				AIngine::Editor::Editor::CreateNewScene();
			}

			ImGui::EndPopup();
		}
	}

	void OpenScenePopUpForLoadScene()
	{
		ImGui::OpenPopup("SaveSceneForLoadScene?");
	}

	void SaveSceneForLoadScenePopUp()
	{
		if (ImGui::BeginPopupModal("SaveSceneForLoadScene?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Do you want to save changes made to the current scene?");

			std::string currentSceneFilepath = AIngine::Editor::Editor::GetCurrentSceneFilePath();
			bool bSceneLoaded = !currentSceneFilepath.empty();
			static const ImVec2 buttonSize(90, 20);

			if (bSceneLoaded) {
				if (ImGui::Button("Save##saveScenePopUp", buttonSize)) {
					AIngine::Editor::Editor::SaveScene(currentSceneFilepath);
					ImGui::CloseCurrentPopup();
					AIngine::Editor::Editor::LoadSceneFromFile();
				}
			}
			else {
				ImGui::InvisibleButton("Save##saveScenePopUp", buttonSize);
			}

			ImGui::SameLine();

			if (ImGui::Button("Save As##saveScenePopup", buttonSize))
			{
				if (AIngine::Editor::Editor::SaveSceneToFile()) {
					ImGui::CloseCurrentPopup();
					AIngine::Editor::Editor::LoadSceneFromFile();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Don't Save##saveScenePopUp", buttonSize))
			{
				ImGui::CloseCurrentPopup();
				AIngine::Editor::Editor::LoadSceneFromFile();
			}

			ImGui::EndPopup();
		}
	}
}