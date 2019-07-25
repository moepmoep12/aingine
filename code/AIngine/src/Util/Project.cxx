#include "Util/Project.h"
#include "Util/SystemCommand.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <Windows.h>

namespace AIngine::Util::Project {

	static const char* PROJECTFILEPATH = "project.proj";

	std::filesystem::path ExePath() {
		char buffer[MAX_PATH];
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::string::size_type pos = std::string(buffer).find_last_of("\\/");
		return std::filesystem::path(std::string(buffer).substr(0, pos));
	}


	std::string GetProjectDir()
	{
		std::ifstream file;
		file.open(PROJECTFILEPATH);
		if (file.fail()) return "";
		nlohmann::json j = nlohmann::json::parse(file);
		file.close();
		std::string path = j.at("path");
		return path;
	}

	std::string GetProjectName()
	{
		std::ifstream file;
		file.open(PROJECTFILEPATH);
		if (file.fail()) return "";
		nlohmann::json j = nlohmann::json::parse(file);
		file.close();

		return j.at("name");
	}

	std::string GetResourceDirectory()
	{
		return GetProjectDir() + "\\Resources\\";
	}

	std::string GetEngineInstallDirectory()
	{
		std::ifstream file;
		file.open(PROJECTFILEPATH);
		if (file.fail()) return "";
		nlohmann::json j = nlohmann::json::parse(file);
		file.close();
		std::string path = j.at("installPath");
		return path;
	}

	void SetProjectDir(const std::string & dir, const std::string& filepath)
	{
		std::ifstream file;
		if (filepath.empty())
			file.open(PROJECTFILEPATH);
		else
			file.open(filepath);
		if (file.fail()) return;
		nlohmann::json j = nlohmann::json::parse(file);
		j.at("path") = std::filesystem::canonical(dir).string();
		file.close();
		std::ofstream f;
		if (filepath.empty())
			f.open(PROJECTFILEPATH);
		else
			f.open(filepath);
		f << j.dump(0);
		f.close();
	}

	void RegenerateCMake(const std::vector<std::string>& variables)
	{
		std::string projectRoot = GetProjectDir();
		std::string cmakeBinPath = std::filesystem::canonical(projectRoot + "out\\CMake").string();
		std::stringstream command;

		// cd to the correct drive
		command << cmakeBinPath[0] << ":" << " &&";
		// move to folder
		command << "cd " << cmakeBinPath << " && ";
		command << "cmake ";
		for (auto& var : variables)
			command << var << " ";
		command << projectRoot;

		AIngine::Util::System::Exec({ command.str() });
	}
}