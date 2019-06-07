#include "Editor/Widgets/MenuBarWidget.h"
#include "Editor/Editor.h"
#include "Editor/Serialization.h"
#include "AIngine/World.h"
#include "Structures/SceneGraph.h"

#include "Debug/log.h"

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

				if (ImGui::MenuItem("Open")) {

					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialog(filterList, NULL, &outPath);

					if (result == NFD_OKAY)
					{
						AIngine::Editor::Editor::SetCurrentScene(std::string(outPath));
						// delete the old tree
						AIngine::Editor::Editor::ResetSceneGraph();
						AIngine::Editor::Serialization::Serializer::DeserializeSceneGraph(AIngine::Editor::Editor::GetCurrentScene());
						free(outPath);
					}
				}

				if (ImGui::MenuItem("Save", "STRG + S", false, !AIngine::Editor::Editor::GetCurrentScene().empty()))
				{
					AIngine::Editor::Serialization::Serializer::SerializeSceneGraph(AIngine::Editor::Editor::GetCurrentScene());
				}

				if (ImGui::MenuItem("Save As"))
				{
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_SaveDialog(filterList, NULL, &outPath);

					if (result == NFD_OKAY)
					{
						AIngine::Editor::Editor::SetCurrentScene(std::string(outPath));
						// delete the old tree
						AIngine::Editor::Serialization::Serializer::SerializeSceneGraph(AIngine::Editor::Editor::GetCurrentScene());
						free(outPath);
					}
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