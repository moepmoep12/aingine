#pragma once
#include "AIngine/Core.h"
#include "Project.h"

namespace ProjectLauncher {

	class Launcher : public AIngine::Application {
	public:
		Launcher();
		virtual ~Launcher();

		// Inherited via Application
		virtual void OnAppStartUp() override;
		virtual void OnAppShutDown() override;
		virtual void OnAppUpdate() override;
		virtual void OnAppEvent(AIngine::Events::EventData& e) override {}

		static bool ContainsProject(const std::string& name);
		static bool IsProjectDir(const std::string& path);
		static std::vector<Project>* GetProjects();
		static void CreateNewProject(const std::string& name, const std::string& path, unsigned int bit = 64);
		//static void LoadProject(const std::string& path);
		static void AddProject(const std::string& name, const std::string& projectRoot);
		static void OpenProject(const Project& proj);
		static bool HasCmakeBinaries(const std::string& projectRoot, const std::string& projectName);
		static bool HasCmakeBinaries(const std::string& projectFilePath);
		static void RegenerateCMake(const std::string& projectRoot);
		static std::string GetProjectRootFromProjectFile(const std::string& projectFilePath);
		static std::string GetProjectNameFromProjectFile(const std::string& projectFilePath);
		static void RunCMake(const std::string& name, const std::string& path, unsigned int bit);
		static void OpenVSSolution(const std::string& name, const std::string& path);
		static void CreateDirectories(const std::string& path);
		static void CopyDirectories(const std::string& path);

	private:
		void LoadProjectsFromFile();
		void SaveProjectsToFile();
		static void CreateTemplateFiles(const std::string& name, const std::string& path);
		static void CreateProjectFile(const std::string& name, const std::string& path);
		static void CreateScriptingFile(const std::string& path);

	private:
		static Launcher* s_instance;
		static const char* PROJECTSFILEPATH;
		std::vector<Project> m_projects;
		std::string m_installpath;
	};
}