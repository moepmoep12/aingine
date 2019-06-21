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
			<< "#include" <<'"'<< "AIngine/Core.h" <<'"' << '\n'
			<< '\n'
			<< "namespace " << projectName << " {" << '\n'
			<< "	class " << projectName << " : public AIngine::Application {" << '\n'
			<< "	public:" << '\n'
			<< "		" << projectName << "();" << '\n'
			<< "		// Inherited via Application" << '\n'
			<< "		virtual void OnAppStartUp() override;" <<'\n'
			<< "		virtual void OnAppShutDown() override;" << '\n'
			<< "		virtual void OnAppUpdate() override;" << '\n'
			<< "		virtual void OnAppEvent(AIngine::Events::EventData& e) override;" << '\n'
			<<"	};" << '\n'
			<<"}";

		return ss.str();
	}

	inline std::string GetSourceTemplate(const std::string& projectName) 
	{
		std::stringstream ss;
		ss
			<< "#include " << '"' << projectName <<  << '"'


		return ss.str();
	}

}
