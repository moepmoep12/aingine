#include "Util/Project.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace AIngine::Util::Project {

	static const char* PROJECTFILEPATH = "project.proj";

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
}