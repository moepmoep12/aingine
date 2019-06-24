#pragma once
#include "Launcher.h"
#include "EntryPoint.h"
#include "LauncherGUI.h"
#include "TemplateFiles.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <ctime>
#include <filesystem>
#include <stdlib.h>


AIngine::Application* AIngine::CreateApplication() {
	return new ProjectLauncher::Launcher();
}

// deprecated. will delete soon
std::vector<std::string> AIngine::ApplicationComponentNames = {};
void AIngine::OnAddComponent(AIngine::GameObject* obj, int index) {}

namespace ProjectLauncher {

	Launcher* Launcher::s_instance = nullptr;

	static const glm::vec2 s_windowSize = glm::vec2(600, 400);

	Launcher::Launcher()
	{
		s_instance = this;

		GLFWmonitor* primary = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primary);
		m_window->SetWindowSize(s_windowSize.x, s_windowSize.y);
		m_window->SetWindowTitle("Launcher");
		m_window->SetWindowSizeLimits(s_windowSize.x, s_windowSize.y, s_windowSize.x, s_windowSize.y);
		m_window->SetWindowResize(false);
		PushOverlay(new LauncherGUI());

		LoadProjectsFromFile();
		m_installpath = Application::GetInstallPath();
	}

	Launcher::~Launcher()
	{
		SaveProjectsToFile();
	}

	void Launcher::OnAppStartUp()
	{
		static const glm::vec2 monitorRes = m_window->GetMonitorResolution();
		static glm::vec2 pos = monitorRes * 0.5f;
		pos -= s_windowSize;
		m_window->SetWindowPosition(pos);
	}

	void Launcher::OnAppShutDown()
	{
	}

	void Launcher::OnAppUpdate()
	{
		// keep constant window size
		if (m_window->GetWidth() != s_windowSize.x || m_window->GetHeight() != s_windowSize.y)
			m_window->SetWindowSize(s_windowSize.x, s_windowSize.y);
	}


	const char* Launcher::PROJECTSFILEPATH = "Projects.json";

	namespace AttributeNames {

		const char* PROJECT_NAME = "name";
		const char* PROJECT_PATH = "path";
		const char* PROJECT_DATE = "lastUsed";
		const char* PROJECT_SCRIPTS = "scripts";
		const char* ENGINE_INSTALLPATH = "installPath";
	}

	bool Launcher::ContainsProject(const std::string & name)
	{
		if (s_instance) {
			for (auto& project : s_instance->m_projects)
			{
				if (project.Name == name)
					return true;
			}
		}
		return false;
	}

	bool Launcher::IsProjectDir(const std::string & path)
	{
		if (!std::filesystem::is_directory(path)) return false;

		if (s_instance) {
			for (auto& project : s_instance->m_projects)
			{
				if (project.AbsolutePath == path)
					return true;
			}
		}
		return false;
	}

	std::vector<Project>* Launcher::GetProjects()
	{
		if (s_instance)
			return &s_instance->m_projects;

		return nullptr;
	}

	void Launcher::CreateNewProject(const std::string & name, const std::string & path, unsigned int bit)
	{
		if (s_instance)
		{
			std::string projectPath = path + "\\" + name + "\\";

			// Create folders
			CreateDirectories(projectPath);

			// copy folders
			//CopyDirectories(projectPath);

			// Create Files
			CreateTemplateFiles(name, projectPath);

			// Create Project File
			CreateProjectFile(name, projectPath);

			// Create Scripting File
			CreateScriptingFile(projectPath + "code\\src\\");

			// run cmake build
			RunCMake(name, projectPath, bit);

			// open VS Solution
			OpenVSSolution(name, projectPath);

			s_instance->m_projects.push_back(Project
				{
					name,
					std::filesystem::canonical(projectPath).string()
				});
		}
	}

	void Launcher::LoadProject(const std::string & path)
	{
		if (std::filesystem::exists(path)) {
			using json = nlohmann::json;

			std::ifstream file;
			file.open(path);
			if (file.fail()) return;

			json j = json::parse(file);
			file.close();

			s_instance->m_projects.push_back(Project
				{
					j[AttributeNames::PROJECT_NAME],
					j[AttributeNames::PROJECT_PATH]
				});
		}
	}

	void Launcher::OpenProject(const Project & proj)
	{
		if (s_instance) {
			OpenVSSolution(proj.Name, proj.AbsolutePath);
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
			std::string path = project[AttributeNames::PROJECT_PATH];
			if (std::filesystem::is_directory(std::filesystem::path(path))) {
				m_projects.push_back(Project
					{
						project[AttributeNames::PROJECT_NAME],
						project[AttributeNames::PROJECT_PATH]
					});
			}
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
			outer.push_back(j);
		}

		std::ofstream file;
		file.open(PROJECTSFILEPATH);
		file << outer.dump(0);
		file.close();
	}
	void Launcher::CreateDirectories(const std::string & path)
	{
		// create folders
		std::string codePath = path + "code";

		std::filesystem::create_directories(std::filesystem::path(codePath + "\\include"));
		std::filesystem::create_directories(std::filesystem::path(codePath + "\\src"));
		std::filesystem::create_directories(std::filesystem::path(path + "lib"));
		std::filesystem::create_directories(std::filesystem::path(path + "Resources"));
		std::filesystem::create_directories(std::filesystem::path(path + "out\\CMake"));
	}
	void Launcher::CopyDirectories(const std::string & path)
	{
		std::string resourceDir = Application::GetResourceDirectory();
		std::string libDir = resourceDir + "Launcher\\lib";
		std::string engineResDir = resourceDir + "Launcher\\AIngine";
		static const std::filesystem::copy_options copyOptions = std::filesystem::copy_options::recursive | std::filesystem::copy_options::update_existing;
		std::filesystem::copy(libDir, path + "lib", copyOptions);
		std::filesystem::copy(engineResDir, path + "Resources\\AIngine", copyOptions);
	}

	void Launcher::CreateTemplateFiles(const std::string & name, const std::string & path)
	{
		std::string header = GetHeaderTemplate(name);
		std::string src = GetSourceTemplate(name);
		std::string cmakeList = GetCMakeListTemplate(name, Application::GetInstallPath());

		std::string headerFilePath = path + "code\\include\\" + name + ".h";
		std::string sourceFilePath = path + "code\\src\\" + name + ".cxx";
		std::string cmakelistFilePath = path + "CMakeLists.txt";

		std::ofstream file;
		file.open(headerFilePath);
		file << header;
		file.close();

		file.open(sourceFilePath);
		file << src;
		file.close();

		file.open(cmakelistFilePath);
		file << cmakeList;
		file.close();
	}

	void Launcher::CreateProjectFile(const std::string & name, const std::string & path)
	{
		nlohmann::json j;
		j[AttributeNames::PROJECT_NAME] = name;
		j[AttributeNames::PROJECT_PATH] = std::filesystem::canonical(path).string() + "\\";
		j[AttributeNames::PROJECT_SCRIPTS] = std::vector<std::string>();
		j[AttributeNames::ENGINE_INSTALLPATH] = std::filesystem::canonical(s_instance->m_installpath).string() + "\\";
		std::ofstream file;
		file.open(path + name + ".proj");
		file << j.dump(0);
		file.close();
	}

	void Launcher::CreateScriptingFile(const std::string & path)
	{
		std::string p = path + "\\Scripting_generated.cxx";
		std::ofstream file;
		file.open(p);
		file
			<< "#include \"AIngine/Core.h\"" << '\n'
			<< "#include \"AIngine/GameObject.h\"" << '\n'
			<< "#include <vector>" << '\n'
			<< "#include <string>" << '\n'
			<< '\n'
			<< "std::vector<std::string> AIngine::ApplicationComponentNames = {" << '\n'
			<< "};" << '\n'
			<< '\n'
			<< "void AIngine::OnAddComponent(AIngine::GameObject* obj, int index) {" << '\n'
			<< "	switch (index) {" << '\n'
			<< "	}" << '\n'
			<< "}";
		file.close();
	}

	void Launcher::RunCMake(const std::string & name, const std::string & path, unsigned int bit)
	{
		std::string cmakeBinPath = path + "out\\CMake";
		std::stringstream command;
		// cd to the correct drive
		command << path[0] << ":" << " &&";
		// move to folder
		command << "cd " << cmakeBinPath << " && ";
		// run cmake
		command << "cmake " << "-G \"Visual Studio 15 2017 ";
		if (bit == 64)
			command << "Win" << bit;
		command << "\" " << " -DCMAKE_INSTALL_PREFIX=" << std::filesystem::canonical(Application::GetInstallPath()).string() << " " << path;

		system(command.str().c_str());
	}

	void Launcher::OpenVSSolution(const std::string & name, const std::string & path)
	{
		std::string cmakeBinPath = path + "\\out\\CMake";
		std::stringstream command;
		// cd to the correct drive
		command << path[0] << ":" << " &&";
		// move to folder
		command << "cd " << cmakeBinPath << " && ";
		command << name << ".sln";
		system(command.str().c_str());
	}
}