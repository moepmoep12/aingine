#pragma once

#include <string>
#include <vector>

namespace AIngine::Editor::Scripting {

	void AddScript(const std::string& name);
	void RemoveScript(const std::string& name);

	std::string GenerateExternScriptMethods(const std::vector<std::string>& scriptNames, const std::string& projectname);

	std::string GetScriptTemplateHeader(const std::string& name);
	std::string GetScriptTemplateSource(const std::string& name);

	std::vector<std::string> GetScriptNames();
}