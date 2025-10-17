#pragma once

#include <poc.core/log.hpp>

#define POC_WORKSPACE_TRACE(...) POC_LOGGER_TRACE(poc::workspace::logger(), __VA_ARGS__)
#define POC_WORKSPACE_DEBUG(...) POC_LOGGER_DEBUG(poc::workspace::logger(), __VA_ARGS__)
#define POC_WORKSPACE_INFO(...) POC_LOGGER_INFO(poc::workspace::logger(), __VA_ARGS__)
#define POC_WORKSPACE_WARN(...) POC_LOGGER_WARN(poc::workspace::logger(), __VA_ARGS__)
#define POC_WORKSPACE_ERROR(...) POC_LOGGER_ERROR(poc::workspace::logger(), __VA_ARGS__)
#define POC_WORKSPACE_CRITICAL(...) POC_LOGGER_CRITICAL(poc::workspace::logger(), __VA_ARGS__)

namespace poc::workspace {

auto logger() -> log::logger*;

} // namespace poc::workspace
