#include "LauncherGUI.h"
#include "Launcher.h"
#include "AIngine/Core.h"

#include "imgui.h"


namespace ProjectLauncher {

	static Project* s_selectedProject = nullptr;

	void LauncherGUI::OnImGuiRender()
	{
		static bool open = true;
		static float color[] = { 1,1,1,1 };
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 15));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.25, 0.25, 0.25, 1));

		CreateDockSpace(true);

		if (ImGui::Begin("Projects", &open, 0))
		{
			ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
			ImGui::BeginChild("ProjectsChildWindow", ImVec2(0, 300), childFlags);
			for (auto& project : *Launcher::GetProjects()) {

				static const ImVec2 selectableSize = ImVec2(0, 40);
				std::stringstream ss;

				ss << project.Name << '\n' << '\n';
				ss << "   " << project.AbsolutePath;

				bool selected = s_selectedProject == &project;

				ImGui::Selectable(ss.str().c_str(), &selected, 0, selectableSize);

				if (ImGui::IsItemClicked(0)) {
					s_selectedProject = &project;
				}
			}

			ImGui::PopStyleVar();
			ImGui::EndChildFrame();

			ImGui::Separator();

			static const ImVec2 buttonSize = ImVec2(120, 60);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));

			ImGui::SetCursorPosY(325);

			if (ImGui::Button("Create", buttonSize)) {

			}

			ImGui::SameLine();

			if (s_selectedProject)
			{
				if (ImGui::Button("Open", buttonSize)) {

				}
			}
			else {
				ImGui::InvisibleButton("Open", buttonSize);
			}

			ImGui::SameLine();

			if (s_selectedProject) {
				if (ImGui::Button("Delete", buttonSize))
				{
					auto it = Launcher::GetProjects()->begin();

					while (it != Launcher::GetProjects()->end()) {
						if (it._Ptr == s_selectedProject) {
							Launcher::GetProjects()->erase(it);
							break;
						}
					}
				}
			}

			else {
				ImGui::InvisibleButton("Delete", buttonSize);
			}

			ImGui::PopStyleVar();

		}
		ImGui::PopStyleColor();
		ImGui::End();
	}

	void LauncherGUI::CreateDockSpace(bool show)
	{
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &show, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		ImGui::End();
	}
}