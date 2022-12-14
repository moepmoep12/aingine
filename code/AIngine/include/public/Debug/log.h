#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/logger.h"
//#include "spdlog/fmt/ostr.h"
//#include <mutex>
//#include "spdlog/details/null_mutex.h"
//#include "spdlog/sinks/base_sink.h"
#include "UI/ImGuiLayer.h"

namespace  AIngine {

	class Log final {

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
}


//#ifdef EDITOR
	#define CORE_ERROR(...)  ::AIngine::Log::GetCoreLogger()->error(__VA_ARGS__)
	#define CORE_WARN(...)   ::AIngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
	#define CORE_TRACE(...)  ::AIngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
	#define CORE_INFO(...)   ::AIngine::Log::GetCoreLogger()->info(__VA_ARGS__)
//#else
//	#define CORE_ERROR(...)
//	#define CORE_WARN(...)
//	#define CORE_TRACE(...)
//	#define CORE_INFO(...)
////#define CORE_FATAL(...)  ::AIngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)
//#endif

//#ifdef EDITOR
	#define DEBUG_ERROR(...)  ::AIngine::Log::GetClientLogger()->error(__VA_ARGS__)
	#define DEBUG_WARN(...)   ::AIngine::Log::GetClientLogger()->warn(__VA_ARGS__)
	#define DEBUG_TRACE(...)  ::AIngine::Log::GetClientLogger()->trace(__VA_ARGS__)
	#define DEBUG_INFO(...)   ::AIngine::Log::GetClientLogger()->info(__VA_ARGS__)
//#else 
//	#define DEBUG_ERROR(...)  
//	#define DEBUG_WARN(...)   
//	#define DEBUG_TRACE(...)
//	#define DEBUG_INFO(...)
////#define DEBUG_FATAL(...)  ::AIngine::Log::GetClientLogger()->fatal(__VA_ARGS__)
//#endif
