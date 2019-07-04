#include "Editor/ScriptingAPI.h"
#include "Util/Project.h"

#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <filesystem>


namespace AIngine::Editor::Scripting {

	void AddScript(const std::string & name)
	{
		static const std::string projectname = AIngine::Util::Project::GetProjectName();
		static const std::string projectDir = AIngine::Util::Project::GetProjectDir();
		static const std::string cmakeListFilePath = projectDir + "CMakeLists.txt";

		const std::string header = "code/include/${PROJECT_NAME}.h";
		const std::string src = "code/src/${PROJECT_NAME}.cxx";
		const std::string insertHeader = "    code/include/" + name + ".h" + '\n';
		const std::string insertSrc = "    code/src/" + name + ".cxx" + '\n';

		// open the file
		std::ifstream cmakeListsFile;
		cmakeListsFile.open(cmakeListFilePath);
		if (cmakeListsFile.fail()) return;

		std::stringstream lines;
		std::string line;

		// Read the cmkaelist line by line and insert the new script.h && .cxx file
		while (std::getline(cmakeListsFile, line)) {
			if (line.find(header) != std::string::npos)
				lines << insertHeader;
			if (line.find(src) != std::string::npos)
				lines << insertSrc;
			lines << line << '\n';
		}
		cmakeListsFile.close();

		// write back into the file
		std::ofstream f;
		f.open(cmakeListFilePath);
		f << lines.str();
		f.close();

		std::string headerPath = projectDir + "code\\include\\" + name + ".h";
		std::string sourcePath = projectDir + "code\\src\\" + name + ".cxx";

		// Create the new files
		f.open(headerPath);
		f << GetScriptTemplateHeader(name);
		f.close();

		f.open(sourcePath);
		f << GetScriptTemplateSource(name);
		f.close();

		// Update extern Script methods
		std::vector<std::string> componentNames;
		std::string projectFilePath = AIngine::Util::Project::GetProjectDir() + projectname + ".proj.in";
		std::ifstream projectFile;
		projectFile.open(projectFilePath);
		if (projectFile.fail()) return;
		// read existing scripts in the project
		nlohmann::json j = nlohmann::json::parse(projectFile);
		j.at("scripts").push_back(name);
		projectFile.close();

		for (auto& scriptname : j.at("scripts")) {
			componentNames.push_back(scriptname);
		}

		f.open(projectFilePath);
		f << j.dump(0);
		f.close();

		f.open(projectDir + "code\\src\\Scripting_generated.cxx");
		f << GenerateExternScriptMethods(componentNames, projectname);
		f.close();

		// Refresh project
		AIngine::Util::Project::RegenerateCMake({});
	}

	void RemoveScript(const std::string & name)
	{
		static const std::string projectname = AIngine::Util::Project::GetProjectName();
		static const std::string projectDir = AIngine::Util::Project::GetProjectDir();
		std::string projectFilePath = AIngine::Util::Project::GetProjectDir() + projectname + ".proj.in";
		std::vector<std::string> componentNames;

		std::ifstream projectFile;
		projectFile.open(projectFilePath);
		if (projectFile.fail()) return;
		// read existing scripts in the project
		nlohmann::json j = nlohmann::json::parse(projectFile);
		projectFile.close();

		for (auto& scriptname : j.at("scripts")) {
			if (scriptname != name)
				componentNames.push_back(scriptname);
		}

		// replace the list of scripts without the script being removed
		j.at("scripts") = componentNames;

		// write back the list of scripts to the project file
		std::ofstream f;
		f.open(projectFilePath);
		f << j.dump(0);
		f.close();

		// update the extern script methods
		f.open(projectDir + "code\\src\\Scripting_generated.cxx");
		f << GenerateExternScriptMethods(componentNames, projectname);
		f.close();

		// open the cmakelist
		static const std::string cmakeListFilePath = projectDir + "CMakeLists.txt";
		std::ifstream cmakeListsFile;
		cmakeListsFile.open(cmakeListFilePath);
		if (cmakeListsFile.fail()) return;

		std::stringstream lines;
		std::string line;

		// Read the cmakelist line by line and skip the line with the script toRemove
		while (std::getline(cmakeListsFile, line)) {
			if (line.find(name) == std::string::npos)
				lines << line << '\n';
		}
		cmakeListsFile.close();

		// write back into the cmakelist
		f.open(cmakeListFilePath);
		f << lines.str();
		f.close();

		// Delete the script files
		std::filesystem::remove(projectDir + "code\\include\\" + name + ".h");
		std::filesystem::remove(projectDir + "code\\src\\" + name + ".cxx");

		// Refresh project
		AIngine::Util::Project::RegenerateCMake({});
	}

	std::string GenerateExternScriptMethods(const std::vector<std::string>& scriptNames, const std::string& projectname)
	{
		std::stringstream Template;
		Template
			<< "#include \"AIngine/Core.h\"" << '\n'
			<< "#include \"AIngine/GameObject.h\"" << '\n';
		for (auto& scriptname : scriptNames) {
			Template << "#include \"" << scriptname << ".h\"" << '\n';
		}
		Template
			<< "#include <vector>" << '\n'
			<< "#include <string>" << '\n'
			<< '\n'
			<< "std::vector<std::string> AIngine::ApplicationComponentNames = {" << '\n';
		for (auto& scriptname : scriptNames) {
			Template << "    " << '"' << scriptname << "\"," << '\n';
		}
		Template
			<< "};" << '\n'
			<< '\n'
			<< "void AIngine::OnAddComponent(AIngine::GameObject* obj, int index) {" << '\n'
			<< "	switch (index) {" << '\n';
		int i = 0;
		for (auto& scriptname : scriptNames) {
			Template
				<< "	case " << i << ":" << '\n'
				<< "		obj->AddComponent<" << projectname << "::" << scriptname << ">()->ScriptIndex = " << i << ";" << '\n'
				<< "		break;" << '\n'
				<< '\n';
			i++;
		}
		Template
			<< "	}" << '\n'
			<< "}";

		return Template.str();
	}


	std::string GetScriptTemplateHeader(const std::string & name)
	{
		std::stringstream ss;
		ss
			<< "#pragma once" << '\n'
			<< "#include \"AIngine/Core.h\"" << '\n'
			<< '\n'
			<< "namespace " + AIngine::Util::Project::GetProjectName() + " {" << '\n'
			<< "	class " + name + " : public AIngine::Script {" << '\n'
			<< "	public:" << '\n'
			<< "		" + name + "();" << '\n'
			<< "		// Inherited via Script" << '\n'
			<< "		virtual void OnStart() override;" << '\n'
			<< "		virtual void OnEnd() override;" << '\n'
			<< "		virtual void Update(float delta) override;" << '\n'
			<< "		virtual void OnEventData(AIngine::Events::EventData& e) override;" << '\n'
			<< "	};" << '\n'
			<< "}";

		return ss.str();
	}

	std::string GetScriptTemplateSource(const std::string & name)
	{
		std::stringstream ss;
		ss
			<< "#include \"" + name + ".h\"" << '\n'

			<< "namespace " + AIngine::Util::Project::GetProjectName() + " {" << '\n'
			<< '\n'
			<< "	// Constructor" << '\n'
			<< "	" << name << "::" << name << "()" << '\n'
			<< "	{" << '\n'
			<< "		// In order for the editor to display the scripts name correctly" << '\n'
			<< "		SetName(typeid(*this).name());" << '\n'
			<< "	}" << '\n'
			<< '\n'
			<< "	// Start is called when gameplay starts for this script" << '\n'
			<< "	void " + name + "::OnStart()" << '\n'
			<< "	{" << '\n'
			<< "	}" << '\n'
			<< '\n'
			<< "	// End is called when gameplay ends for this script" << '\n'
			<< "	void " + name + "::OnEnd()" << '\n'
			<< "	{" << '\n'
			<< "	}" << '\n'
			<< '\n'
			<< "	// Update is called once per frame" << '\n'
			<< "	void " + name + "::Update(float delta)" << '\n'
			<< "	{" << '\n'
			<< "	}" << '\n'
			<< '\n'
			<< "	// Callback for events" << '\n'
			<< "	void " + name + "::OnEventData(AIngine::Events::EventData & e)" << '\n'
			<< "	{" << '\n'
			<< "	}" << '\n'
			<< "}";
		return ss.str();
	}

	std::vector<std::string> GetScriptNames()
	{
		static const std::string projectname = AIngine::Util::Project::GetProjectName();
		static const std::string projectFilePath = AIngine::Util::Project::GetProjectDir() + projectname + ".proj.in";
		std::vector<std::string> componentNames;

		std::ifstream projectFile;
		projectFile.open(projectFilePath);
		if (projectFile.fail()) return componentNames;
		// read existing scripts in the project
		nlohmann::json j = nlohmann::json::parse(projectFile);
		projectFile.close();

		for (auto& scriptname : j.at("scripts")) {
			componentNames.push_back(scriptname);
		}

		return componentNames;
	}
}