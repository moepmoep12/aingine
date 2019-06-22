#pragma once

#include <string>
#include <sstream>

namespace ProjectLauncher {

	inline std::string GetHeaderTemplate(const std::string& projectName)
	{
		std::stringstream ss;
		ss
			<< "#pragma once" << '\n'
			<< '\n'
			<< "#include" << '"' << "AIngine/Core.h" << '"' << '\n'
			<< '\n'
			<< "namespace " << projectName << " {" << '\n'
			<< "	class " << projectName << " : public AIngine::Application {" << '\n'
			<< "	public:" << '\n'
			<< "		" << projectName << "();" << '\n'
			<< "		// Inherited via Application" << '\n'
			<< "		virtual void OnAppStartUp() override;" << '\n'
			<< "		virtual void OnAppShutDown() override;" << '\n'
			<< "		virtual void OnAppUpdate() override;" << '\n'
			<< "		virtual void OnAppEvent(AIngine::Events::EventData& e) override;" << '\n'
			<< "	};" << '\n'
			<< "}";

		return ss.str();
	}

	inline std::string GetSourceTemplate(const std::string& projectName)
	{
		std::stringstream ss;
		ss
			<< "#include " << '"' << projectName << ".h" << '"' << '\n'
			<< "#include \"EntryPoint.h\" // Do not remove" << '\n'
			<< '\n'
			<< "// Define the extern declaration" << '\n'
			<< "AIngine::Application* AIngine::CreateApplication() {" << '\n'
			<< "	return new " << projectName << "::" << projectName << "();" << '\n'
			<< "}" << '\n'
			<< '\n'
			<< "// deprecated, will be deleted soon" << '\n'
			<< "std::vector<std::string> AIngine::ApplicationComponentNames = {};" << '\n'
			<< "void AIngine::OnAddComponent(AIngine::GameObject* obj, int index) {}" << '\n'
			<< '\n'
			<< "namespace " << projectName << " {" << '\n'
			<< '\n'
			<< "	// Constructor" << '\n'
			<< "	" << projectName << "::" << projectName << "()" << '\n'
			<< "	{"
			<< "		// Configure the physics world with a gravity of 10 downwards" << '\n'
			<< "		m_gravity = glm::vec2(0, 10);" << '\n'
			<< '\n'
			<< "		// Set the bounds of the world to be 10 in width and 10 in height" << '\n'
			<< "		m_bounds = glm::vec4(0.0, 10.0, 0.0, 10.0);" << '\n'
			<< '\n'
			<< "		// Set the window title" << '\n'
			<< "		m_window->SetWindowTitle(" << '"' << projectName << '"' << ");" << '\n'
			<< '\n'
			<< "		// Set the window size to be the whole screen" << '\n'
			<< "		const glm::vec2 monitorResolution = m_window->GetMonitorResolution();" << '\n'
			<< "		m_window->SetWindowSize(monitorResolution.x, monitorResolution.y);" << '\n'
			<< "	}" << '\n'
			<< '\n'
			<< "	// StartUp is called after the scene and engine subsystems have been initialized" << '\n'
			<< "	void " << projectName << "::OnAppStartUp()" << '\n'
			<< "	{" << '\n'
			<< "	}" << '\n'
			<< '\n'
			<< "	// ShutDown is called before the scene and engine subystems have been shut down" << '\n'
			<< "	void " << projectName << "::OnAppShutDown()" << '\n'
			<< "	{" << '\n'
			<< "	}" << '\n'
			<< '\n'
			<< "	// Update is called once per frame" << '\n'
			<< "	void " << projectName << "::OnAppUpdate()" << '\n'
			<< "	{" << '\n'
			<< "	}" << '\n'
			<< '\n'
			<< "	// Here you can react to events" << '\n'
			<< "	void " << projectName << "::OnAppEvent(AIngine::Events::EventData & e)" << '\n'
			<< "	{" << '\n'
			<< "	}" << '\n'
			<< "}";


		return ss.str();
	}

	inline std::string GetCMakeListTemplate(const std::string& projectname) {
		std::stringstream ss;
		ss
			<< "cmake_minimum_required(VERSION 3.13 FATAL_ERROR)" << '\n'
			<< "project(" << projectname << "VERSION 1.0 LANGUAGES CXX)" << '\n'
			<< '\n'
			<< "set(CMAKE_MODULE_PATH ${ CMAKE_MODULE_PATH } \"${CMAKE_SOURCE_DIR}/out/CMake/Modules/\")" << '\n'
			<< '\n'
			<< '\n'
			<< "#--- Source Files" << '\n'
			<< "set(SRC" << '\n'
			<< "    code / src / ${ PROJECT_NAME }.cxx" << '\n'
			<< "     )" << '\n'
			<< '\n'
			<< "set(HEADER" << '\n'
			<< "	   code / include / ${ PROJECT_NAME }.h" << '\n'
			<< "    )" << '\n'
			<< '\n'
			<< "#" << '\n'
			<< "# --- Output Configuration" << '\n'
			<< "#" << '\n'
			<< "set(OUTPUT_DIR \"${ CMAKE_CURRENT_SOURCE_DIR } / out\")" << '\n'
			<< "set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY \"${ OUTPUT_DIR } / lib\")" << '\n'
			<< "set(CMAKE_LIBRARY_OUTPUT_DIRECTORY \"${ OUTPUT_DIR } / lib\")" << '\n'
			<< "set(CMAKE_RUNTIME_OUTPUT_DIRECTORY \"${ OUTPUT_DIR } / bin\")" << '\n'
			<< '\n'
			<< "# Add Executable Target" << '\n'
			<< "add_executable(${ PROJECT_NAME } ${ HEADER } ${ SRC })" << '\n'
			<< '\n'
			<< "#--- LIBRARIES" << '\n'
			<< "link_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib)" << '\n'
			<< "target_link_libraries(${ PROJECT_NAME } ${ CMAKE_CURRENT_SOURCE_DIR } / lib / AIngine / Engine.lib)" << '\n'
			<< "target_link_libraries(${ PROJECT_NAME } ${ CMAKE_CURRENT_SOURCE_DIR } / lib / Box2D - cmake / Box2D - cmake.lib)" << '\n'
			<< "target_link_libraries(${ PROJECT_NAME } ${ CMAKE_CURRENT_SOURCE_DIR } / lib / freetype2 / freetype.lib)" << '\n'
			<< "target_link_libraries(${ PROJECT_NAME } ${ CMAKE_CURRENT_SOURCE_DIR } / lib / glad / glad.lib)" << '\n'
			<< "target_link_libraries(${ PROJECT_NAME } ${ CMAKE_CURRENT_SOURCE_DIR } / lib / glfw / glfw3.lib)" << '\n'
			<< "target_link_libraries(${ PROJECT_NAME } ${ CMAKE_CURRENT_SOURCE_DIR } / lib / imgui / imgui.lib)" << '\n'
			<< "target_link_libraries(${ PROJECT_NAME } ${ CMAKE_CURRENT_SOURCE_DIR } / lib / nativefiledialog - cmake / nativefiledialog.lib)" << '\n'
			<< "target_link_libraries(${ PROJECT_NAME } ${ CMAKE_CURRENT_SOURCE_DIR } / lib / tinyobjloader / tinyobjloader.lib)" << '\n'
			<< "target_link_libraries(${ PROJECT_NAME } ${ CMAKE_CURRENT_SOURCE_DIR } / lib / xcsrlib / xcsrlib.lib)" << '\n'
			<< '\n'
			<< "#--- LIBRARIES HEADER" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / AIngine / public)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / Box2D - cmake / Box2D)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / cute_sound)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / freetype2 / include)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / glad / include)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / glfw / include)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / glm)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / imgui)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / json / include)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / nativefiledialog - cmake / src / include)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / spdlog / include)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / stb)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / tinyobjloader)" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / lib / xcsrlib / include)" << '\n'
			<< '\n'
			<< "#-- Add include path" << '\n'
			<< "include_directories(${ CMAKE_CURRENT_SOURCE_DIR } / code / include)" << '\n'
			<< '\n'
			<< '\n'
			<< "#--- PROPERTIES" << '\n'
			<< "set_target_properties(${ PROJECT_NAME } PROPERTIES" << '\n'
			<< "CXX_STANDARD 17" << '\n'
			<< "CXX_EXTENSIONS OFF)" << '\n'
			<< '\n'
			<< "#" << '\n'
			<< "# --- Code Editor Configuration" << '\n'
			<< "#" << '\n'
			<< "if (WIN32)" << '\n'
			<< "# Configure VS Project" << '\n'
			<< "set_target_properties(${ PROJECT_NAME }" << '\n'
			<< "PROPERTIES" << '\n'
			<< "VS_DEBUGGER_WORKING_DIRECTORY \"$(OutDir)\")" << '\n'
			<< "set_property(DIRECTORY ${ CMAKE_CURRENT_SOURCE_DIR }" << '\n'
			<< "PROPERTY" << '\n'
			<< "VS_STARTUP_PROJECT ${ PROJECT_NAME })" << '\n'
			<< "endif()" << '\n'
			<< '\n'
			<< '\n'
			<< "# Set as Startup Project" << '\n'
			<< "set_property(DIRECTORY ${ CMAKE_CURRENT_SOURCE_DIR } PROPERTY VS_STARTUP_PROJECT ${ PROJECT_NAME })" << '\n'
			<< '\n'
			<< "#-- Copy project File" << '\n'
			<< "configure_file(${ CMAKE_CURRENT_SOURCE_DIR } / ${ PROJECT_NAME }.proj ${ CMAKE_RUNTIME_OUTPUT_DIRECTORY } / project.proj COPYONLY)";

		return ss.str();
	}
}
