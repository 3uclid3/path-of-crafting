#include <poc/log.hpp>

#include <memory>
#include <mutex>
#include <vector>

#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <poc/assert.hpp>

namespace poc::log {

#if SPDLOG_ACTIVE_LEVEL != SPDLOG_LEVEL_OFF
namespace details {

log::options options;
std::vector<spdlog::sink_ptr> sinks;

auto make_logger(const std::string& name) -> std::shared_ptr<logger>
{
    POC_ASSERT(!sinks.empty());
    return std::make_shared<logger>(name, sinks.begin(), sinks.end());
}

auto create_sinks() -> void
{
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(1024));

    if (!options.file.empty())
    {
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(options.file, 1024 * 1024 * 5, 3, true));
    }
}

auto create_default_logger() -> void
{
    auto default_logger = make_logger("");

    default_logger->set_level(options.level);
    default_logger->flush_on(options.flush_on_level);

    spdlog::set_default_logger(default_logger);
}

} // namespace details

auto init(const options& options) -> void
{
    details::options = options;

    spdlog::set_level(details::options.level);
    spdlog::flush_on(details::options.flush_on_level);

    details::create_sinks();
    details::create_default_logger();

    POC_INFO("Log level: {}", spdlog::level::to_string_view(spdlog::default_logger()->level()));
}

auto uninit() -> void
{
    details::sinks.clear();

    spdlog::shutdown();
}

auto make(const std::string& name) -> std::shared_ptr<logger>
{
    auto new_logger = details::make_logger(name);

    spdlog::initialize_logger(new_logger);

    POC_TRACE("Logger {} added with log level {}", name, spdlog::level::to_string_view(new_logger->level()));

    return new_logger;
}

#else

auto init(const options&) -> void
{
}

auto uninit() -> void
{
}

auto make(const std::string&) -> std::shared_ptr<logger>
{
    static std::shared_ptr<logger> null_logger = std::make_shared<logger>("null");
    return null_logger;
}

#endif // SPDLOG_ACTIVE_LEVEL != SPDLOG_LEVEL_OFF

} // namespace poc::log
