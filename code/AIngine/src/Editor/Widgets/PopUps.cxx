#include "Editor/Widgets/PopUps.h"
#include "Editor/Editor.h"
#include "Editor/ScriptingAPI.h"

#include "imgui.h"
#include <nfd.h>

namespace AIngine::Editor::PopUps {

	void OpenPopUpForNewScene()
	{
		ImGui::OpenPopup("SaveSceneForNewScene?");
	}

	void CreatePopUpForSavingChangesMadeSceneWhenCreatingNewScene()
	{
		if (ImGui::BeginPopupModal("SaveSceneForNewScene?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Do you want to save changes made to the current scene?");

			std::string currentSceneFilepath = AIngine::Editor::Editor::GetCurrentSceneFilePath();
			bool bSceneLoaded = !currentSceneFilepath.empty();
			static const ImVec2 buttonSize(90, 20);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));

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
			ImGui::PopStyleVar();
			ImGui::EndPopup();
		}
	}

	void OpenPopUpForLoadScene()
	{
		ImGui::OpenPopup("SaveSceneForLoadScene?");
	}

	void CreatePopUpForSavingChangesMadeSceneWhenLoadingScene()
	{
		if (ImGui::BeginPopupModal("SaveSceneForLoadScene?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Do you want to save changes made to the current scene?");

			std::string currentSceneFilepath = AIngine::Editor::Editor::GetCurrentSceneFilePath();
			bool bSceneLoaded = !currentSceneFilepath.empty();
			static const ImVec2 buttonSize(90, 20);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));

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
			ImGui::PopStyleVar();
			ImGui::EndPopup();
		}
	}

	void OpenPopUpForBuildScenes()
	{
		ImGui::OpenPopup("BuildScenes?");
	}

	Scene* s_selectedScene = nullptr;
	void CreatePopUpForBuildScenes()
	{
		if (ImGui::BeginPopupModal("BuildScenes?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			std::vector<AIngine::Editor::Scene>* scenes = AIngine::Editor::Editor::GetBuildScenes();
			if (scenes) {
				if (!s_selectedScene && scenes->size() > 0) s_selectedScene = &(*scenes)[0];

				for (auto& scene : *scenes)
				{
					if (ImGui::Selectable(scene.Name.c_str())) {
						s_selectedScene = &scene;
					}
				}

				ImGui::NewLine();
				ImGui::NewLine();
				static ImVec2 buttonSize(40, 20);
				std::string currentScene = AIngine::Editor::Editor::GetCurrentSceneName();

				if (!currentScene.empty() && !AIngine::Editor::Editor::ContainsScene(currentScene)) {
					if (ImGui::Button("Add CurrentScene##BuildScene", buttonSize)) {
						AIngine::Editor::Editor::AddCurrentSceneToBuild();
					}
				}

				else ImGui::InvisibleButton("Add CurrentScene##BuildScene", buttonSize);

				ImGui::SameLine();
				if (ImGui::ArrowButton("MoveSceneUp", ImGuiDir_Up))
				{
					if (s_selectedScene)
					{
						int index = 0;
						int i = 0;
						for (auto& scene : *scenes) {
							if (s_selectedScene == &scene) {
								index = i - 1;
								break;
							}
							i++;
						}

						if (index < scenes->size()) {
							Scene temp = (*scenes)[index];
							Scene toMoveUp = *s_selectedScene;
							(*scenes)[index] = toMoveUp;
							(*scenes)[index + 1] = temp;
						}
					}
				}

				ImGui::SameLine();
				if (ImGui::ArrowButton("MoveSceneDown", ImGuiDir_Down))
				{
					if (s_selectedScene) {

					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Save")) {
					AIngine::Editor::Editor::SaveBuildScenes();
				}

				ImGui::SameLine();

				if (ImGui::Button("Close##SceneBuild", buttonSize))
				{
					ImGui::CloseCurrentPopup();

				}


				ImGui::EndPopup();
			}
		}
	}

	void OpenPopUpForEnterScriptName()
	{
		ImGui::OpenPopup("PickScriptName");
	}

	void CreatePopUpForEnterScriptName()
	{
		if (ImGui::BeginPopupModal("PickScriptName", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Enter Script name");

			static char str[40] = "NewScript";

			if (ImGui::InputText("", str, IM_ARRAYSIZE(str)));

			if (ImGui::Button("Create##newscript"))
			{
				AIngine::Editor::Scripting::AddScript(str);
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void OpenPopUpForScriptManagement()
	{
		ImGui::OpenPopup("ManageScripts");
	}

	void CreatePopUpForScriptManagement()
	{
		static int selectedIndex = 0;

		if (ImGui::BeginPopupModal("ManageScripts", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static const ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
			ImGui::BeginChild("Scripts##child", ImVec2(0, 300), false, childFlags);
			std::vector<std::string> scripts = AIngine::Editor::Scripting::GetScriptNames();

			for (int i = 0; i < scripts.size(); i++) {
				static const ImVec2 selectableSize = ImVec2(0, 20);

				bool isSelected = selectedIndex == i;

				ImGui::Selectable(scripts[i].c_str(), &isSelected, 0, selectableSize);

				if (ImGui::IsItemClicked(0)) {
					selectedIndex = i;
				}
			}

			ImGui::EndChildFrame();

			ImGui::Separator();

			static const ImVec2 buttonSize = ImVec2(100, 50);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20, 20));

			if (ImGui::Button("Create##script", buttonSize)) {
				OpenPopUpForEnterScriptName();
			}
			CreatePopUpForEnterScriptName();

			ImGui::SameLine();

			if (ImGui::Button("Remove##script", buttonSize)) {
				if (scripts.size() > 0 && selectedIndex < scripts.size()) {
					Scripting::RemoveScript(scripts[selectedIndex]);
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Close##managescript", buttonSize)) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::PopStyleVar();
			ImGui::EndPopup();
		}
	}
}