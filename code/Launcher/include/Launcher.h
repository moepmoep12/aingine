#pragma once
#include "AIngine/Core.h"
#include "Project.h"

namespace ProjectLauncher {

	class Launcher : public AIngine::Application {
	public:
		Launcher();

		// Inherited via Application
		virtual void OnAppStartUp() override;
		virtual void OnAppShutDown() override;
		virtual void OnAppUpdate() override;
		virtual void OnAppEvent(AIngine::Events::EventData& e) override {}

		static std::vector<Project>* GetProjects();
		static void CreateNewProject(const std::string& name, const std::string& path);

	private:
		void LoadProjectsFromFile();
		void SaveProjectsToFile();

	private:
		static Launcher* s_instance;
		static const char* PROJECTSFILEPATH;
		std::vector<Project> m_projects;
	};
}