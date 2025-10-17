#include <poc.platform/log.hpp>

namespace poc::platform {

auto logger() -> log::logger*
{
    static const auto logger = log::make_logger("platform");
    return logger.get();
}

} // namespace poc::platform