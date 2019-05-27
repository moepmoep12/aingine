#include "Editor/Widgets/MenuBarWidget.h"

#include "AIngine/World.h"

namespace AIngine::Editor {
	void MenubarWidget::OnImGuiRender()
	{
		if (ImGui::BeginMainMenuBar()) {

			m_size = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
			glm::vec2 pos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
			m_position = pos;

			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItem("Open")) {
					
				}

				if (ImGui::MenuItem("Save")) {

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
				static bool show = false;

				if (ImGui::MenuItem("Show Physics Debug Rendering","F1", &show, true)) 
				{
					AIngine::World::SetPhysicsDebugDrawActive(show);
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