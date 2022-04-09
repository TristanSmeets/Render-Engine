#include "Rendererpch.h"
#include "Log.h"

void Log::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	spdlog::set_level(spdlog::level::trace);
}

void Log::Close()
{
	spdlog::shutdown();
}
