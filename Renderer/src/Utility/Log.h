#pragma once
#include "spdlog/spdlog.h"
#include <memory>

class Log {
public:
	static void Init();
	static void Close();
	
	template<typename T> inline static void Info(const T& msg) 
	{
		spdlog::info(msg); 
	}

	template<typename... Args> static inline void Info(spdlog::format_string_t<Args...> fmt, Args&& ...args) 
	{
		spdlog::info(fmt, std::forward<Args>(args)...); 
	}
	
	template<typename T> inline static void Warn(const T& msg)
	{ 
		spdlog::warn(msg);
	}
	
	template<typename... Args> inline static void Warn(spdlog::format_string_t<Args...> fmt, Args&& ...args) 
	{
		spdlog::warn(fmt, std::forward<Args>(args)...); 
	}
	
	template<typename T> inline static void Error(const T& msg)
	{
		spdlog::error(msg);
	}

	template<typename... Args> static inline void Error(spdlog::format_string_t<Args...> fmt, Args&& ...args)
	{
		spdlog::error(fmt, std::forward<Args>(args)...); 
	}

	template<typename T> inline static void Debug(const T& msg)
	{ 
		spdlog::debug(msg); 
	}

	template<typename... Args> static inline void Debug(spdlog::format_string_t<Args...> fmt, Args&& ...args)
	{
		spdlog::debug(fmt, std::forward<Args>(args)...);
	}

	template<typename T> inline static void Trace(const T& msg) 
	{ 
		spdlog::trace(msg);
	}
	template<typename... Args> static inline void Trace(spdlog::format_string_t<Args...> fmt, Args&& ...args)
	{
		spdlog::trace(fmt, std::forward<Args>(args)...);
	}
};
