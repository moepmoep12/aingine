#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/logger.h"
#include "spdlog/fmt/ostr.h"
#include <mutex>
#include "spdlog/details/null_mutex.h"
#include "spdlog/sinks/base_sink.h"
#include "UI/ImGuiLayer.h"
#include "UI/LogWidget.h"

namespace  AIngine {

	class Log {

	public:
		Log();
		~Log();

		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_clientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;

	};

	template<typename Mutex>
	class LogWidgetSink : public spdlog::sinks::base_sink <Mutex>
	{
	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{
			fmt::memory_buffer formatted;
			sink::formatter_->format(msg, formatted);

			AIngine::UI::ImGuiLayer::s_logWidget.AddLog(fmt::to_string(formatted).c_str());
		}

		void flush_() override {
			AIngine::UI::ImGuiLayer::s_logWidget.Clear();
		}
	};

	using LogWidgetSink_mt = LogWidgetSink<std::mutex>;
	using LogWidgetSink_st = LogWidgetSink<spdlog::details::null_mutex>;
}


#ifdef _DEBUG
	#define CORE_ERROR(...)  ::AIngine::Log::GetCoreLogger()->error(__VA_ARGS__)
	#define CORE_WARN(...)   ::AIngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
	#define CORE_TRACE(...)  ::AIngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
	#define CORE_INFO(...)   ::AIngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#else
	#define CORE_ERROR(...)
	#define CORE_WARN(...)
	#define CORE_TRACE(...)
	#define CORE_INFO(...)
//#define CORE_FATAL(...)  ::AIngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#endif

#ifdef _DEBUG
	#define DEBUG_ERROR(...)  ::AIngine::Log::GetClientLogger()->error(__VA_ARGS__)
	#define DEBUG_WARN(...)   ::AIngine::Log::GetClientLogger()->warn(__VA_ARGS__)
	#define DEBUG_TRACE(...)  ::AIngine::Log::GetClientLogger()->trace(__VA_ARGS__)
	#define DEBUG_INFO(...)   ::AIngine::Log::GetClientLogger()->info(__VA_ARGS__)
#else 
	#define DEBUG_ERROR(...)  
	#define DEBUG_WARN(...)   
	#define DEBUG_TRACE(...)
	#define DEBUG_INFO(...)
//#define DEBUG_FATAL(...)  ::AIngine::Log::GetClientLogger()->fatal(__VA_ARGS__)
#endif

#ifdef _DEBUG
	#define ASSERT(x,...) {if(!(x)) {CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define ASSERT(x,...)
#endif