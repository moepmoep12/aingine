#pragma once

#include <string>
#include <chrono>

namespace ProjectLauncher {

	struct Project {
		std::string Name;
		std::string AbsolutePath;
		//std::chrono::time_point<std::chrono::system_clock> LastUsed;
	};
}