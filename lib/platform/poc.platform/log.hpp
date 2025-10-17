#pragma once

#include <poc.core/log.hpp>

#define POC_PLATFORM_TRACE(...) POC_LOGGER_TRACE(poc::platform::logger(), __VA_ARGS__)
#define POC_PLATFORM_DEBUG(...) POC_LOGGER_DEBUG(poc::platform::logger(), __VA_ARGS__)
#define POC_PLATFORM_INFO(...) POC_LOGGER_INFO(poc::platform::logger(), __VA_ARGS__)
#define POC_PLATFORM_WARN(...) POC_LOGGER_WARN(poc::platform::logger(), __VA_ARGS__)
#define POC_PLATFORM_ERROR(...) POC_LOGGER_ERROR(poc::platform::logger(), __VA_ARGS__)
#define POC_PLATFORM_CRITICAL(...) POC_LOGGER_CRITICAL(poc::platform::logger(), __VA_ARGS__)

namespace poc::platform {

auto logger() -> log::logger*;

} // namespace poc::platform
