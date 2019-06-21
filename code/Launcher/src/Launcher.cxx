#pragma once
#include "Launcher.h"
#include "EntryPoint.h"
#include "LauncherGUI.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <ctime>
#include <filesystem>

AIngine::Application* AIngine::CreateApplication() {
	return new ProjectLauncher::Launcher();
}

// deprecated. will delete soon
std::vector<std::string> AIngine::ApplicationComponentNames = {};
void AIngine::OnAddComponent(AIngine::GameObject* obj, int index) {}

namespace ProjectLauncher {

	Launcher* Launcher::s_instance = nullptr;

	Launcher::Launcher()
	{
		s_instance = this;

		GLFWmonitor* primary = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primary);
		m_window->SetWindowSize(600, 400);
		m_window->SetWindowTitle("Launcher");
		m_window->SetWindowSizeLimits(600, 600, 400, 400);
		m_window->SetWindowResize(false);
		PushOverlay(new LauncherGUI());
	}

	void Launcher::OnAppStartUp()
	{
		LoadProjectsFromFile();
	}

	void Launcher::OnAppShutDown()
	{
		SaveProjectsToFile();
	}

	void Launcher::OnAppUpdate()
	{
		// keep constant window size
		if (m_window->GetWidth() != 600 || m_window->GetHeight() != 400)
			m_window->SetWindowSize(600, 400);
	}


	const char* Launcher::PROJECTSFILEPATH = "Projects.json";

	namespace AttributeNames {

		const char* PROJECT_NAME = "name";
		const char* PROJECT_PATH = "path";
		const char* PROJECT_DATE = "lastUsed";
	}

	std::vector<Project>* Launcher::GetProjects()
	{
		if (s_instance)
			return &s_instance->m_projects;

		return nullptr;
	}

	void Launcher::CreateNewProject(const std::string & name, const std::string & path)
	{
		if (s_instance)
		{
			s_instance->m_projects.push_back(Project
				{
					name,
					path
				});

			// copy files

			// run cmake build

			// open VS Solution

			// finally, close the launcher
			s_instance->ShutDown();
		}
	}

	void Launcher::LoadProjectsFromFile()
	{
		using json = nlohmann::json;

		std::ifstream file;
		file.open(PROJECTSFILEPATH);
		if (file.fail()) return;

		json j = json::parse(file);
		file.close();

		for (auto& project : j) {
			m_projects.push_back(Project
				{
					project[AttributeNames::PROJECT_NAME],
					project[AttributeNames::PROJECT_PATH]
				});
		}
	}

	void Launcher::SaveProjectsToFile()
	{
		nlohmann::json outer;

		for (auto& project : m_projects)
		{
			nlohmann::json j;
			j[AttributeNames::PROJECT_NAME] = project.Name;
			j[AttributeNames::PROJECT_PATH] = std::filesystem::canonical(project.AbsolutePath).string();
			//std::time_t time = std::chrono::system_clock::to_time_t(project.LastUsed);
			//j[AttributeNames::PROJECT_DATE] = std::ctime(&time);
			outer.push_back(j);
		}

		std::ofstream file;
		file.open(PROJECTSFILEPATH);
		file << outer.dump(0);
		file.close();
	}
}