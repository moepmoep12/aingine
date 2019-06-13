#include "Editor/Widgets/MenuBarWidget.h"
#include "Editor/Editor.h"
#include "Editor/Serialization.h"
#include "AIngine/World.h"
#include "Structures/SceneGraph.h"
#include "Editor/Widgets/PopUps.h"

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
					PopUps::OpenScenePopUpForNewScene();
				}
				PopUps::SaveSceneForNewScenePopUp();

				// load scene
				ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
				if (ImGui::MenuItem("Open")) {

					PopUps::OpenScenePopUpForLoadScene();
				}
				PopUps::SaveSceneForLoadScenePopUp();

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

				ImGui::EndMenu();
			}

			static bool play = AIngine::Editor::Editor::GetIsInPlayMode();
			if (ImGui::Checkbox("Play", &play)) {
				AIngine::Editor::Editor::SetIsInPlayMode(play);
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