#include "log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace AIngine {

	std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger> Log::s_clientLogger;

	Log::~Log()
	{
		CORE_INFO("Destructor Logging");
	}

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_coreLogger = spdlog::stdout_color_mt("Core");
		s_coreLogger->set_level(spdlog::level::trace);
		
		s_clientLogger = spdlog::stdout_color_mt("App");
		s_clientLogger->set_level(spdlog::level::trace);


#ifdef LOG_TO_FILE
		auto sink = (spdlog::sink_ptr) std::make_shared < spdlog::sinks::basic_file_sink_mt>("log.txt",true);
		s_coreLogger->sinks().push_back(sink);
		s_clientLogger->sinks().push_back(sink);
#endif
	}
}