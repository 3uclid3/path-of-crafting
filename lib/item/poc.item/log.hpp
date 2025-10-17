#pragma once

#include <poc.core/log.hpp>

#define POC_ITEM_TRACE(...) POC_LOGGER_TRACE(poc::item_logger(), __VA_ARGS__)
#define POC_ITEM_DEBUG(...) POC_LOGGER_DEBUG(poc::item_logger(), __VA_ARGS__)
#define POC_ITEM_INFO(...) POC_LOGGER_INFO(poc::item_logger(), __VA_ARGS__)
#define POC_ITEM_WARN(...) POC_LOGGER_WARN(poc::item_logger(), __VA_ARGS__)
#define POC_ITEM_ERROR(...) POC_LOGGER_ERROR(poc::item_logger(), __VA_ARGS__)
#define POC_ITEM_CRITICAL(...) POC_LOGGER_CRITICAL(poc::item_logger(), __VA_ARGS__)

namespace poc {

auto item_logger() -> log::logger*;

} // namespace poc
