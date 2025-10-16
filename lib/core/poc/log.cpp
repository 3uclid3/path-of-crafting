#include <poc/log.hpp>

#include <memory>
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
    POC_ASSERT(!sinks.empty());

    auto default_logger = std::make_shared<logger>("", details::sinks.begin(), details::sinks.end());

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
}

auto uninit() -> void
{
    details::sinks.clear();

    spdlog::shutdown();
}

auto make_logger(const string& name) -> std::shared_ptr<logger>
{
    POC_ASSERT(!details::sinks.empty());

    auto new_logger = std::make_shared<logger>(name, details::sinks.begin(), details::sinks.end());

    spdlog::initialize_logger(new_logger);

    return new_logger;
}

#else

auto init(const options&) -> void
{
}

auto uninit() -> void
{
}

auto make_logger(const std::string&) -> std::shared_ptr<logger>
{
    static std::shared_ptr<logger> null_logger = std::make_shared<logger>("null");
    return null_logger;
}

#endif // SPDLOG_ACTIVE_LEVEL != SPDLOG_LEVEL_OFF

} // namespace poc::log
