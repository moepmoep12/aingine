#include "Util/SystemCommand.h"
#include "Debug/log.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <sstream>

void AIngine::Util::System::Exec(const std::vector<std::string>& commands)
{
	std::array<char, 128> buffer;
	std::string result;
	std::stringstream ss;

	// concatenate commands to a single command
	for (int i = 0; i < commands.size(); i++) {
		ss << commands[i];
		if (i < commands.size() - 1)
			ss << " && ";
	}
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(ss.str().c_str(), "r"), _pclose);

	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}

	CORE_INFO(result);
}
