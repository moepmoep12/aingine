#include "Debug/log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "Editor/Widgets/LogWidget.h"

namespace AIngine {

	std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger> Log::s_clientLogger;

	Log::Log()
	{
	}

	Log::~Log()
	{
		CORE_INFO("Destructor Logging");
	}

	void Log::Init()
	{
		CreateDirectory("Editor", NULL);

		spdlog::set_pattern("%^[%T] %n: %v%$");

		auto logSink = (spdlog::sink_ptr) std::make_shared<AIngine::Editor::Widget::LogWidgetSink_st>();

		s_coreLogger = spdlog::stdout_color_mt("Core");
		s_coreLogger->set_level(spdlog::level::trace);

		s_clientLogger = spdlog::stdout_color_mt("App");
		s_clientLogger->set_level(spdlog::level::trace);

		s_coreLogger->sinks().push_back(logSink);
		s_clientLogger->sinks().push_back(logSink);


#ifdef LOG_TO_FILE
		auto sink = (spdlog::sink_ptr) std::make_shared < spdlog::sinks::basic_file_sink_mt>("Editor/log.txt", true);
		s_coreLogger->sinks().push_back(sink);
		s_clientLogger->sinks().push_back(sink);
#endif
	}


	//void LogWidgetSink::sink_it_(const spdlog::details::log_msg& msg)  {
	//	fmt::memory_buffer formatted;
	//	sink::formatter_->format(msg, formatted);

	//	AIngine::UI::ImGuiLayer::s_logWidget.AddLog(fmt::to_string(formatted).c_str());
	//}
}