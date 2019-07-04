#pragma once
#include "Launcher.h"
#include "EntryPoint.h"
#include "LauncherGUI.h"
#include "TemplateFiles.h"
#include "Util/Project.h"
#include "Util/SystemCommand.h"

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
		m_installpath = AIngine::Util::Project::GetEngineInstallDirectory();
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
		//const char* PROJECT_DATE = "lastUsed";
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
			CopyDirectories(projectPath);

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

	//void Launcher::LoadProject(const std::string & path)
	//{
	//	if (std::filesystem::exists(path)) {
	//		using json = nlohmann::json;

	//		// the path ends with the project file, which we don't want
	//		std::string p = std::filesystem::canonical(path).string();
	//		int lastIndex = p.find_last_of('\\');
	//		if (lastIndex > 0) {
	//			p = p.substr(0, lastIndex);
	//		}

	//		std::ifstream file;
	//		file.open(path);
	//		if (file.fail()) return;
	//		json j = json::parse(file);
	//		file.close();

	//		// update the paths
	//		//j[AttributeNames::PROJECT_PATH] = std::filesystem::canonical(p).string() + "\\";
	//		//j[AttributeNames::ENGINE_INSTALLPATH] = std::filesystem::canonical(s_instance->m_installpath).string() + "\\";

	//		s_instance->m_projects.push_back(Project
	//			{
	//				j[AttributeNames::PROJECT_NAME],
	//				j[AttributeNames::PROJECT_PATH]
	//			});

	//		//// write back the updates
	//		//std::ofstream writeFile;
	//		//writeFile.open(path);
	//		//if (writeFile.fail()) return;
	//		//writeFile << j.dump(0);
	//		//writeFile.close();
	//	}
	//}

	void Launcher::AddProject(const std::string & name, const std::string & projectRoot)
	{
		s_instance->m_projects.push_back(Project
			{
				name,
				std::filesystem::canonical(projectRoot).string() + "\\"
			});
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

	bool Launcher::HasCmakeBinaries(const std::string & projectRoot, const std::string& projectName)
	{
		if (!std::filesystem::is_directory(projectRoot)) return false;

		const std::string cmakeBinDir = projectRoot + "\\out\\CMake\\";

		bool cmakeDirExists = std::filesystem::is_directory(cmakeBinDir);
		if (!cmakeDirExists) return false;

		const std::string VSsolutionFile = cmakeBinDir + projectName + ".sln";
		bool solutionFileExists = std::filesystem::exists(VSsolutionFile);

		return solutionFileExists;
	}

	bool Launcher::HasCmakeBinaries(const std::string & projectFilePath)
	{
		std::ifstream file;
		file.open(projectFilePath);
		if (file.fail()) return false;
		nlohmann::json j = nlohmann::json::parse(file);
		file.close();

		return HasCmakeBinaries(j[AttributeNames::PROJECT_PATH], j[AttributeNames::PROJECT_NAME]);
	}

	void Launcher::RegenerateCMake(const std::string & projectRoot)
	{
		std::string cmakeBinPath = std::filesystem::canonical(projectRoot + "\\out\\CMake").string();
		std::stringstream command;
		// cd to the correct drive
		command << cmakeBinPath[0] << ":" << " &&";
		// move to folder
		command << "cd " << cmakeBinPath << " && ";
		// run cmake
		command << "cmake -DCMAKE_INSTALL_PREFIX=" << std::filesystem::canonical(AIngine::Util::Project::GetEngineInstallDirectory()).string() << " " << projectRoot;

		AIngine::Util::System::Exec({ command.str() });
	}

	std::string Launcher::GetProjectRootFromProjectFile(const std::string & projectFilePath)
	{
		std::ifstream file;
		file.open(projectFilePath);
		if (file.fail()) return std::string();
		nlohmann::json j = nlohmann::json::parse(file);
		file.close();

		if (j.contains(AttributeNames::PROJECT_PATH))
		{
			std::string path = j[AttributeNames::PROJECT_PATH];
			return path;
		}

		return std::string();
	}

	std::string Launcher::GetProjectNameFromProjectFile(const std::string & projectFilePath)
	{
		std::ifstream file;
		file.open(projectFilePath);
		if (file.fail()) return std::string();
		nlohmann::json j = nlohmann::json::parse(file);
		file.close();

		if (j.contains(AttributeNames::PROJECT_NAME))
		{
			std::string path = j[AttributeNames::PROJECT_NAME];
			return path;
		}

		return std::string();
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
		std::filesystem::create_directories(std::filesystem::path(path + "out\\bin\\Debug"));
		std::filesystem::create_directories(std::filesystem::path(path + "out\\bin\\Release"));
		std::filesystem::create_directories(std::filesystem::path(path + "out\\bin\\RelWithDebInfo"));

	}
	void Launcher::CopyDirectories(const std::string & path)
	{
		std::string imguiFilePath = AIngine::Util::Project::GetEngineInstallDirectory() + "Resources\\AIngine\\imgui.ini";
		static const std::filesystem::copy_options copyOptions = std::filesystem::copy_options::recursive | std::filesystem::copy_options::update_existing;
		std::filesystem::copy(imguiFilePath, path + "out\\bin\\Debug", copyOptions);
		std::filesystem::copy(imguiFilePath, path + "out\\bin\\Release", copyOptions);
		std::filesystem::copy(imguiFilePath, path + "out\\bin\\RelWithDebInfo", copyOptions);
	}

	void Launcher::CreateTemplateFiles(const std::string & name, const std::string & path)
	{
		std::string header = GetHeaderTemplate(name);
		std::string src = GetSourceTemplate(name);
		std::string cmakeList = GetCMakeListTemplate(name, AIngine::Util::Project::GetEngineInstallDirectory());

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
		j[AttributeNames::PROJECT_PATH] = "@projectDir@";
		j[AttributeNames::PROJECT_SCRIPTS] = std::vector<std::string>();
		j[AttributeNames::ENGINE_INSTALLPATH] = "@InstallPath@";
		std::ofstream file;
		file.open(path + name + ".proj.in");
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
		std::string cmakeBinPath = std::filesystem::canonical(path).string() + "\\out\\CMake";
		std::stringstream command;
		// cd to the correct drive
		command << path[0] << ":" << " && ";
		// move to folder
		command << "cd " << cmakeBinPath << " && ";
		// run cmake
		command << "cmake " << "-G \"Visual Studio 15 2017";
		if (bit == 64)
			command << " Win" << bit;
		command << "\" " << " -DCMAKE_INSTALL_PREFIX=" << std::filesystem::canonical(AIngine::Util::Project::GetEngineInstallDirectory()).string() << " " << path;

		AIngine::Util::System::Exec({ command.str() });
	}

	void Launcher::OpenVSSolution(const std::string & name, const std::string & path)
	{
		std::string cmakeBinPath = std::filesystem::canonical(path).string() + "\\out\\CMake";
		std::stringstream command;
		// cd to the correct drive
		command << path[0] << ":" << " &&";
		// move to folder
		command << "cd " << cmakeBinPath << " && ";
		command << name << ".sln";
		AIngine::Util::System::Exec({ command.str() });
	}
}