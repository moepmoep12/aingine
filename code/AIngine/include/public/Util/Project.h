#pragma once

#include <string>
#include <filesystem>
#include <vector>

namespace AIngine::Util::Project {

	std::filesystem::path ExePath();
	std::string GetProjectDir();
	std::string GetProjectName();
	std::string GetResourceDirectory();
	std::string GetEngineInstallDirectory();
	void SetProjectDir(const std::string& dir, const std::string& filepath = "");

	void RegenerateCMake(const std::vector<std::string>& variables);
}
