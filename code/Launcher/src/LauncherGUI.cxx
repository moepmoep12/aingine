#include "LauncherGUI.h"
#include "Launcher.h"
#include "AIngine/Core.h"

#include "imgui.h"
#include "nfd.h"


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


		if (!ImGui::Begin("Projects", &open, 0))
		{
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			ImGui::End();
			return;
		}
		ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
		ImGui::BeginChild("ProjectsChildWindow", ImVec2(0, 300), childFlags);
		for (auto& project : *Launcher::GetProjects()) {

			static const ImVec2 selectableSize = ImVec2(0, 40);
			std::stringstream ss;

			// project name
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

		static bool bCreatingProject = false;
		static const ImVec2 buttonSize = ImVec2(120, 60);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));

		ImGui::SetCursorPosY(325);

		if (ImGui::Button("Create", buttonSize))
		{
			bCreatingProject = !bCreatingProject;
		}

		// Creating a new project
		if (bCreatingProject) {
			ImGui::OpenPopup("PickProjectName");
			if (ImGui::BeginPopupModal("PickProjectName", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20, 20));

				ImGui::Text("Enter project name");

				static char str[40] = "MyProject";

				ImGui::InputText("", str, IM_ARRAYSIZE(str));

				static const int choosableModes[] = { 64,32 };
				static const char* modeNames[] = { "64bit","32bit" };
				static int currentIndex = 0;

				if (ImGui::BeginCombo("Mode", modeNames[currentIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(modeNames); i++) {
						bool isSelected = choosableModes[i] == choosableModes[currentIndex];
						if (ImGui::Selectable(modeNames[i], isSelected)) {
							currentIndex = i;
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Create##newproject"))
				{
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_PickFolder("", &outPath);

					if (result == NFD_OKAY)
					{
						bCreatingProject = false;
						std::string p;
						p.append(outPath).append("\\").append(str);
						if (!Launcher::IsProjectDir(p))
							Launcher::CreateNewProject(str, outPath, choosableModes[currentIndex]);
						ImGui::CloseCurrentPopup();
						free(outPath);
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel##createproject")) {
					bCreatingProject = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::PopStyleVar();
				ImGui::EndPopup();
			}
		}

		ImGui::SameLine();

		static bool choosingMode = false;
		static std::string projectRoot;
		static std::string projectName;

		if (ImGui::Button("Add existing", buttonSize)) {
			static const nfdchar_t *filterList = "proj.in";
			nfdchar_t *outPath = NULL;
			nfdresult_t result = NFD_OpenDialog(filterList, NULL, &outPath);

			if (result == NFD_OKAY)
			{
				// the path ends with the project file, which we don't want
				std::string p = std::filesystem::canonical(outPath).string();
				int lastIndex = p.find_last_of('\\');
				if (lastIndex > 0) {
					projectRoot = p.substr(0, lastIndex);
					int lastIndexP = p.find_last_of('p');
					projectName = p.substr(lastIndex + 1, (lastIndexP - 2) - lastIndex);
				}
				if (Launcher::HasCmakeBinaries(projectRoot, projectName)) {
					// re-generate cmake
					Launcher::RegenerateCMake(projectRoot);
					Launcher::AddProject(projectName, projectRoot);
				}
				// run cmake 
				else {
					choosingMode = true;
				}
				free(outPath);
			}
		}
		if (choosingMode) {
			ImGui::OpenPopup("PickBitVersion");
			if (ImGui::BeginPopupModal("PickBitVersion", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20, 20));

				ImGui::Text("Choose mode");

				static const int choosableModes[] = { 64,32 };
				static const char* modeNames[] = { "64bit","32bit" };
				static int currentIndex = 0;

				if (ImGui::BeginCombo("Mode", modeNames[currentIndex])) {
					for (int i = 0; i < IM_ARRAYSIZE(modeNames); i++) {
						bool isSelected = choosableModes[i] == choosableModes[currentIndex];
						if (ImGui::Selectable(modeNames[i], isSelected)) {
							currentIndex = i;
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Create##newcmake"))
				{
					Launcher::CreateDirectories(projectRoot + "\\");
					Launcher::CopyDirectories(projectRoot + "\\");
					Launcher::RunCMake(projectName, projectRoot, choosableModes[currentIndex]);
					Launcher::AddProject(projectName, projectRoot);
					ImGui::CloseCurrentPopup();
					choosingMode = false;
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel##choosemode")) {
					ImGui::CloseCurrentPopup();
					choosingMode = false;
				}
				ImGui::PopStyleVar();
				ImGui::EndPopup();
			}
		}

		ImGui::SameLine();

		if (s_selectedProject)
		{
			static bool choosingMode = false;
			if (ImGui::Button("Open", buttonSize))
			{
				if (Launcher::HasCmakeBinaries(s_selectedProject->AbsolutePath, s_selectedProject->Name)) {
					Launcher::RegenerateCMake(s_selectedProject->AbsolutePath);
					Launcher::OpenProject(*s_selectedProject);
				}
				else {
					choosingMode = true;
				}
			}
			if (choosingMode) {
				ImGui::OpenPopup("PickBitVersion");
				if (ImGui::BeginPopupModal("PickBitVersion", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20, 20));

					ImGui::Text("Choose mode");

					static const int choosableModes[] = { 64,32 };
					static const char* modeNames[] = { "64bit","32bit" };
					static int currentIndex = 0;

					if (ImGui::BeginCombo("Mode", modeNames[currentIndex])) {
						for (int i = 0; i < IM_ARRAYSIZE(modeNames); i++) {
							bool isSelected = choosableModes[i] == choosableModes[currentIndex];
							if (ImGui::Selectable(modeNames[i], isSelected)) {
								currentIndex = i;
							}
							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					if (ImGui::Button("Create##newcmake"))
					{
						Launcher::RunCMake(s_selectedProject->Name, s_selectedProject->AbsolutePath, choosableModes[currentIndex]);
						ImGui::CloseCurrentPopup();
						choosingMode = false;
						Launcher::OpenProject(*s_selectedProject);
					}

					ImGui::SameLine();

					if (ImGui::Button("Cancel##choosemode")) {
						ImGui::CloseCurrentPopup();
						choosingMode = false;
					}

					ImGui::PopStyleVar();
					ImGui::EndPopup();
				}
			}


		}
		else {
			ImGui::InvisibleButton("Open", buttonSize);
		}

		ImGui::SameLine();

		// Delete Project
		if (s_selectedProject) {
			if (ImGui::Button("Delete", buttonSize))
			{
				auto it = Launcher::GetProjects()->begin();

				while (it != Launcher::GetProjects()->end()) {
					if (it._Ptr == s_selectedProject) {
						Launcher::GetProjects()->erase(it);
						break;
					}
					it++;
				}
			}
		}

		else {
			ImGui::InvisibleButton("Delete", buttonSize);
		}

		ImGui::PopStyleVar();


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
		ImGui::Begin("Launcher DockSpace", &show, window_flags);
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