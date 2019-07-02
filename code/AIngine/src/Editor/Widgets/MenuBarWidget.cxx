#include "Editor/Widgets/MenuBarWidget.h"
#include "Editor/Editor.h"
#include "Editor/Serialization.h"
#include "AIngine/World.h"
#include "Structures/SceneGraph.h"
#include "Editor/Widgets/PopUps.h"
#include "Editor/ScriptingAPI.h"

#include <string>

namespace AIngine::Editor::Widget {


	void MenubarWidget::OnImGuiRender()
	{
		if (ImGui::BeginMainMenuBar()) {

			m_size = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
			glm::vec2 pos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
			m_position = pos;

			if (ImGui::BeginMenu("Scene"))
			{
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
					PopUps::OpenPopUpForEnterScriptName();
				}
				PopUps::CreatePopUpForEnterScriptName();

				ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
				if (ImGui::MenuItem("Manage Scripts")) {
					PopUps::OpenPopUpForScriptManagement();
				}
				PopUps::CreatePopUpForScriptManagement();

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