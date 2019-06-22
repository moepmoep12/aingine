#include "Editor/Widgets/MenuBarWidget.h"
#include "Editor/Editor.h"
#include "Editor/Serialization.h"
#include "AIngine/World.h"
#include "Structures/SceneGraph.h"
#include "Editor/Widgets/PopUps.h"
#include "Editor/ScriptingAPI.h"

#include <string>
#include <nfd.h>

namespace AIngine::Editor {


	void MenubarWidget::OnImGuiRender()
	{
		if (ImGui::BeginMainMenuBar()) {

			m_size = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
			glm::vec2 pos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
			m_position = pos;

			if (ImGui::BeginMenu("Scene"))
			{
				static const nfdchar_t *filterList = "txt,json";
				// create new scene
				ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
				if (ImGui::MenuItem("New##scene"))
				{
					PopUps::OpenPopUpForNewScene();
				}
				PopUps::CreatePopUpForSavingChangesMadeSceneWhenCreatingNewScene();

				// load scene
				ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
				if (ImGui::MenuItem("Open")) {

					PopUps::OpenPopUpForLoadScene();
				}
				PopUps::CreatePopUpForSavingChangesMadeSceneWhenLoadingScene();

				// save open scene at existing path
				if (ImGui::MenuItem("Save", "STRG + S", false, !AIngine::Editor::Editor::GetCurrentSceneFilePath().empty()))
				{
					AIngine::Editor::Editor::SaveScene(AIngine::Editor::Editor::GetCurrentSceneFilePath());
				}

				// save open scene at new path
				if (ImGui::MenuItem("Save As"))
				{
					AIngine::Editor::Editor::SaveSceneToFile();
				}


				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo")) {

				}

				if (ImGui::MenuItem("Redo")) {

				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				bool showPhys = AIngine::World::IsDebugPhysicsDrawn();
				bool showFps = AIngine::Editor::Editor::IsFramerateDisplayed();
				bool outlineAnimation = AIngine::Rendering::SpriteRenderer::AnimateOutline;
				bool showFPSGraph = AIngine::Editor::Editor::IsFpsGraphVisible();

				if (ImGui::MenuItem("Show Physics Debug Rendering", "F1", &showPhys, true))
				{
					AIngine::World::SetPhysicsDebugDrawActive(showPhys);
				}

				if (ImGui::MenuItem("Show FPS", "F2", &showFps, true))
				{
					AIngine::Editor::Editor::SetShowFramerate(showFps);
				}

				if (ImGui::MenuItem("Show Outline Animation", "F3", &outlineAnimation, true))
				{
					AIngine::Rendering::SpriteRenderer::SetAnimateOutlineActive(outlineAnimation);
				}

				if (ImGui::MenuItem("Show FPS Graph", "F4", &showFPSGraph, true)) {
					AIngine::Editor::Editor::SetShowFpsGraph(showFPSGraph);
				}



				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Build")) {

				ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
				if (ImGui::MenuItem("Scene Build Order")) {
					PopUps::OpenPopUpForBuildScenes();
				}
				PopUps::CreatePopUpForBuildScenes();

				if (ImGui::MenuItem("Build##build")) {
					AIngine::Editor::Editor::Build();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scripts")) {

				ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
				if (ImGui::MenuItem("Add Script")) {
					ImGui::OpenPopup("PickScriptName");
				}

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

					ImGui::EndPopup();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

	}

	MenubarWidget::MenubarWidget()
	{
		m_isDocked = true;
		m_wasSizeChanged = false;
	}
}