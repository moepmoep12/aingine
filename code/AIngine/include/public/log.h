#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/logger.h"
#include "spdlog/fmt/ostr.h"

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
}

// TODO : Remove from release builds

#define CORE_ERROR(...)  ::AIngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_WARN(...)   ::AIngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_TRACE(...)  ::AIngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_INFO(...)   ::AIngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_FATAL(...)  ::AIngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define DEBUG_ERROR(...)  ::AIngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define DEBUG_WARN(...)   ::AIngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define DEBUG_TRACE(...)  ::AIngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define DEBUG_INFO(...)   ::AIngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define DEBUG_FATAL(...)  ::AIngine::Log::GetClientLogger()->fatal(__VA_ARGS__)
