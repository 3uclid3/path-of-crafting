#pragma once

#include <spdlog/spdlog.h>

#include <poc/types.hpp>

#define POC_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define POC_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define POC_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define POC_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define POC_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define POC_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

#define POC_LOGGER_TRACE(logger, ...) SPDLOG_LOGGER_TRACE(logger, __VA_ARGS__)
#define POC_LOGGER_DEBUG(logger, ...) SPDLOG_LOGGER_DEBUG(logger, __VA_ARGS__)
#define POC_LOGGER_INFO(logger, ...) SPDLOG_LOGGER_INFO(logger, __VA_ARGS__)
#define POC_LOGGER_WARN(logger, ...) SPDLOG_LOGGER_WARN(logger, __VA_ARGS__)
#define POC_LOGGER_ERROR(logger, ...) SPDLOG_LOGGER_ERROR(logger, __VA_ARGS__)
#define POC_LOGGER_CRITICAL(logger, ...) SPDLOG_LOGGER_CRITICAL(logger, __VA_ARGS__)

namespace poc::log {

using logger = spdlog::logger;

struct options
{
    string file;
    spdlog::level::level_enum level;
    spdlog::level::level_enum flush_on_level;
};

static inline const options default_options{
    .file = {},
    .level = static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL),
    .flush_on_level = spdlog::level::off};

auto init(const options& options = default_options) -> void;
auto uninit() -> void;

auto make_logger(const string& name) -> std::shared_ptr<logger>;

} // namespace poc::log